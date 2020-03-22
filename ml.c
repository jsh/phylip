/* Version 4.0. (c) Copyright 1993-2013 by the University of Washington.
   Written by Michal Palczewski
   Permission is granted to copy and use this program provided no fee is
   charged for it and provided that this copyright notice is not removed. */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include "ml.h"

#define DEBUG
#define MAKENEWV_DEBUG
/* #define USE_NEW_MAKENEWV */

const double MIN_BRANCH_LENGTH = epsilon/4.0;
const double MIN_ROOT_TYME = -10;

/* TODO check to see which of these are needed here */
long endsite;                           // RSGdebug: Check this.
extern long nextree, which;
extern boolean interleaved, printdata, outgropt, treeprint, dotdiff, transvp;
extern steptr weight, category, alias, location, ally;
extern sequence inputSequences;
extern node** lrsaves;
extern long rcategs;
extern boolean usertree, lngths, smoothit, smoothed, polishing;
boolean inserting;

/* prototypes for unexported functions */
static void ml_tree_smoothall(tree* t, node* p);
static boolean ml_tree_try_insert_thorough(tree*, node*, node*, node **, double*, tree*, tree*);
static boolean ml_tree_try_insert_notthorough(tree *, node *, node *, node**, double*);
void ml_node_reinit(node * n);


void ml_tree_init(tree* t, long nonodes, long spp)
{ /* set up variables in ml_tree */
  generic_tree_init(t, nonodes, spp);
  t->smoothall = ml_tree_smoothall;
  t->insert_ = ml_tree_insert_;
  t->re_move = ml_tree_re_move;
  t->try_insert_ = ml_tree_try_insert_;
  t->do_branchl_on_insert_f = ml_tree_do_branchl_on_insert;
  t->do_branchl_on_re_move_f = ml_tree_do_branchl_on_re_move;
} /* ml_tree_init */


void ml_node_copy(node* srcn, node* destn) // RSGbugfix
{ /* copy an ml_node */
  ml_node *src = (ml_node *)srcn;
  ml_node *dest = (ml_node *)destn;
  assert(srcn);                         // RSGdebug
  assert(destn);                        // RSGdebug
  generic_node_copy(srcn, destn);
  dest->categs = src->categs;
  dest->endsite = src->endsite;
  set_tyme((node*)dest, src->node.tyme);

  if(dest->underflows)                  // RSGbugfix
    memcpy(dest->underflows, src->underflows, src->endsite * sizeof(double));
  else
    assert(src->underflows == NULL);    // RSGdebug
} /* ml_node_copy */


void codon_node_copy(node* srcn, node* destn)
{ /* copy a codon_node */
  codon_node *src = (codon_node *)srcn;
  codon_node *dest = (codon_node *)destn;
  long i, j;
  long oldendsite = dest->ml_node.endsite;

  ml_node_copy(srcn, destn);
  if ( oldendsite != 0 && oldendsite != src->ml_node.endsite )
  {
    ((ml_node*)dest)->freex((ml_node*)dest);
    oldendsite = 0;
  }
  if ( oldendsite == 0 )
    ((ml_node*)dest)->allocx(((ml_node*)dest), ((ml_node*)src)->endsite, ((ml_node*)src)->categs);

  for (i = 0; i < src->ml_node.endsite; i++)
    for (j = 0; j < src->ml_node.categs; j++)
      memcpy(dest->codonx[i][j], src->codonx[i][j], sizeof(csitelike));
} /* codon_node_copy */


void prot_node_copy(node* srcn, node* destn)
{
  prot_node *src = (prot_node *)srcn;
  prot_node *dest = (prot_node *)destn;
  long i, j;
  long oldendsite = dest->ml_node.endsite;

  ml_node_copy(srcn, destn);
  if ( oldendsite != 0 && oldendsite != src->ml_node.endsite )
  {
    ((ml_node*)dest)->freex((ml_node*)dest);
    oldendsite = 0;
  }
  if ( oldendsite == 0 )
    ((ml_node*)dest)->allocx(((ml_node*)dest), ((ml_node*)src)->endsite, ((ml_node*)src)->categs);

  for (i = 0; i < src->ml_node.endsite; i++)
    for (j = 0; j < src->ml_node.categs; j++)
      memcpy(dest->x[i][j], src->x[i][j], sizeof(psitelike));
}


void dna_node_copy(node* srcn, node* destn)
{
  dna_node * src  = (dna_node *)srcn;
  dna_node * dest = (dna_node *)destn;
  long i, j;
  long oldendsite = dest->ml_node.endsite;

  ml_node_copy(srcn, destn);

  if ( oldendsite != 0 && oldendsite != src->ml_node.endsite )
  {
    dest->ml_node.freex((ml_node*)dest);
    dest->ml_node.endsite = 0;
  }
  if ( oldendsite == 0 )
    ((ml_node*)dest)->allocx(((ml_node*)dest), ((ml_node*)src)->endsite, ((ml_node*)src)->categs);
  for (i = 0; i < ((ml_node*)src)->endsite; i++)
    for (j = 0; j < ((ml_node*)src)->categs; j++)
      memcpy(((dna_node*)dest)->x[i][j], ((dna_node*)src)->x[i][j], sizeof(sitelike));
}


void fix_x(dna_node* p, long site, double maxx, long rcategs)
{ /* used in  Dnaml, Dnamlk */
  long i, j;
  ((ml_node*)p)->underflows[site] += log(maxx);

  for ( i = 0 ; i < rcategs ; i++ )
  {
    for ( j = 0 ; j < ((long)T - (long)A + 1) ; j++)
      p->x[site][i][j] /= maxx;
  }
} /* fix_x */


void fix_protx(prot_node* p, long site, double maxx, long rcategs)
{ /* used in Proml, Promlk */
  long i, m;

  ((ml_node*)p)->underflows[site] += log(maxx);

  for ( i = 0 ; i < rcategs  ; i++ )
    for (m = 0; m <= 19; m++)
      p->x[site][i][m] /= maxx;
} /* fix_protx */


node * dna_node_new(node_type type, long index) // RSGbugfix
{
  node* n = Malloc(sizeof(dna_node));

  // RSGdebug: "index" should be > 0 if used for array access.  Can be 0 only
  // for initialization where it will be changed to > 0 before used for access.
  // Test here is for ">= 0", which allows both cases.
  assert(index >= 0);

  dna_node_init(n, type, index);
  return n;
}


void dna_node_init(node *node, node_type type, long index)
{
  dna_node *n = (dna_node *)node;

  // RSGdebug: "index" should be > 0 if used for array access.  Can be 0 only
  // for initialization where it will be changed to > 0 before used for access.
  // Test here is for ">= 0", which allows both cases.
  assert(index >= 0);

  ml_node_init(node, type, index);
  n->ml_node.allocx = dna_node_allocx;
  n->ml_node.node.copy = dna_node_copy;
  n->ml_node.node.init = dna_node_init;
  n->ml_node.freex = dna_node_freex;
  n->x = NULL;

  if ( endsite != 0 && rcategs != 0 )
    n->ml_node.allocx((ml_node*)n, endsite, rcategs);
}


node * prot_node_new(node_type type, long index) // RSGbugfix
{
  node *n = Malloc(sizeof(struct prot_node));
  prot_node_init(n, type, index);
  return n;
}


void prot_node_init(node *n, node_type type, long index)
{
  prot_node *pn = (prot_node *)n;

  ml_node_init(n, type, index);
  pn->ml_node.allocx = prot_node_allocx;
  pn->ml_node.node.copy = prot_node_copy;
  pn->ml_node.node.init = prot_node_init;
  pn->ml_node.freex = prot_node_freex;
  pn->x = NULL;
  if ( endsite != 0 && rcategs != 0 )
    pn->ml_node.allocx(&(pn->ml_node), endsite, rcategs);
}


node * codon_node_new(node_type type, long index) // RSGbugfix
{
  node *n = Malloc(sizeof(struct codon_node));
  codon_node_init(n, type, index);
  return n;
}


void codon_node_init(node *n, node_type type, long index)
{
  codon_node *pn = (codon_node *)n;

  ml_node_init(n, type, index);
  pn->ml_node.allocx = codon_node_allocx;
  pn->ml_node.node.copy = codon_node_copy;
  pn->ml_node.node.init = codon_node_init;
  pn->ml_node.freex = codon_node_freex;
  pn->codonx = NULL;
  if ( endsite != 0 && rcategs != 0 )
    pn->ml_node.allocx(&(pn->ml_node), endsite, rcategs);
}


void ml_node_free(node **np)
{
  ml_node *n = (ml_node*)*np;
  n->freex(n);
  generic_node_free(np);
}


void ml_node_init(node *n, node_type type, long index)
{
  ml_node *mln = (ml_node *)n;

  // RSGdebug: "index" should be > 0 if used for array access.  Can be 0 only
  // for initialization where it will be changed to > 0 before used for access.
  // Test here is for ">= 0", which allows both cases.
  assert(index >= 0);

  generic_node_init(n, type, index);
  n->copy = ml_node_copy;
  n->init = ml_node_init;
  n->free = ml_node_free;
  n->reinit = ml_node_reinit;
  n->node_print_f = ml_node_print;
  mln->freex = NULL;                    /* x is only defined for dna_node and prot_node */
  mln->node.tyme = 0;
}


void ml_node_reinit(node * n)
{
  ml_node * mln = (ml_node*)n;
  mln->node.tyme = 0;
  // BUG.970 -- does freex need refreshing ?
  // BUG.970 -- leave for dna_node and prot_node ?
  generic_node_reinit(n);
}


void ml_node_print(node * n)
{
  generic_node_print(n);
  ml_node * mn = (ml_node*)n;
  printf(" ml(endsite:%ld tyme:%lf)", mn->endsite, mn->node.tyme);
}


void allocx(long nonodes, long endsite, long param, ml_node** treenode)
{
  /* param =  sitelength in restml */
  /* param =  rcategs in dnaml/proml */
  long i;
  ml_node *p;
  ml_node *q;

  for (i = 0; i < spp; i++)
    treenode[i]->allocx((ml_node*)treenode[i], endsite, param);
  for (i = spp; i < nonodes; i++)
  {
    p = treenode[i];
    q = p;
    do
    {
      q->allocx(q, endsite, param);
      q = (ml_node*)q->node.next;
    } while ( q != p);
  }
}  /* allocx */


void dna_node_freex(ml_node* n)
{
  dna_node *dn;
  long i;

  dn = (dna_node *)n;
  for ( i = 0 ; i < n->endsite ; i++ )
  {
    free(dn->x[i]);
  }

  free(dn->x);
  dn->x = NULL;
  free(n->underflows);
  n->underflows = NULL;
}


void prot_node_freex(ml_node* n)
{
  prot_node *pn;
  long i;

  pn = (prot_node *)n;
  for ( i = 0 ; i < n->endsite ; i++ )
  {
    free(pn->x[i]);
  }

  free(pn->x);
  pn->x = NULL;
  free(n->underflows);
  n->underflows = NULL;
}


void codon_node_freex(ml_node* n)
{
  codon_node *pn;
  long i;

  pn = (codon_node *)n;
  for ( i = 0 ; i < n->endsite ; i++ )
  {
    free(pn->codonx[i]);
  }

  free(pn->codonx);
  pn->codonx = NULL;
  free(n->underflows);
  n->underflows = NULL;
}


void dna_node_allocx(ml_node* n, long endsite, long rcategs)
{
  dna_node *dn = (dna_node *)n;
  long i;

  dn->x = (phenotype)Malloc(endsite * sizeof(ratelike));
  for ( i = 0 ; i < endsite ; i++ )
    dn->x[i] = (ratelike)Malloc(rcategs * sizeof(sitelike));

  n->categs = rcategs;
  n->endsite = endsite;
  n->underflows = Malloc(endsite * sizeof(double));
}


void prot_node_allocx(ml_node* nn, long endsite, long rcategs)
{
  prot_node *n = (prot_node *)nn;
  long i;

  n->ml_node.categs = rcategs;
  n->ml_node.endsite = endsite;

  n->x = (pphenotype)Malloc(endsite * sizeof(pratelike));
  for ( i = 0 ; i < endsite ; i++ )
    n->x[i] = (pratelike)Malloc(rcategs * sizeof(psitelike));
  n->ml_node.underflows= Malloc(endsite * sizeof(double));
}


void codon_node_allocx(ml_node* nn, long endsite, long rcategs)
{
  codon_node *n = (codon_node *)nn;
  long i;

  n->ml_node.categs = rcategs;
  n->ml_node.endsite = endsite;

  n->codonx = (cphenotype)Malloc(endsite * sizeof(cratelike));
  for ( i = 0 ; i < endsite ; i++ )
    n->codonx[i] = (cratelike)Malloc(rcategs * sizeof(csitelike));
  n->ml_node.underflows= Malloc(endsite * sizeof(double));
}


void makevalues2(long categs, pointarray nodep, long endsite, long spp, sequence y, steptr alias)
{
  /* set up fractional likelihoods at tips */
  /* used by dnaml & dnamlk */
  long i, j, k, l;
  bases b;

  for (k = 0; k < endsite; k++)
  {
    j = alias[k];
    for (i = 0; i < spp; i++)
    {
      for (l = 0; l < categs; l++)
      {
        for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
          ((dna_node*)nodep[i])->x[k][l][(long)b - (long)A] = 0.0;

        switch (y[i][j - 1])
        {
          case 'A':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            break;

          case 'C':
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            break;

          case 'G':
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            break;

          case 'T':
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'U':
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'M':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            break;

          case 'R':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            break;

          case 'W':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'S':
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            break;

          case 'Y':
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'K':
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'B':
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'D':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'H':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)T - (long)A] = 1.0;
            break;

          case 'V':
            ((dna_node*)nodep[i])->x[k][l][0] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)C - (long)A] = 1.0;
            ((dna_node*)nodep[i])->x[k][l][(long)G - (long)A] = 1.0;
            break;

          case 'N':
            for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
              ((dna_node*)nodep[i])->x[k][l][(long)b - (long)A] = 1.0;
            break;

          case 'X':
            for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
              ((dna_node*)nodep[i])->x[k][l][(long)b - (long)A] = 1.0;
            break;

          case '?':
            for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
              ((dna_node*)nodep[i])->x[k][l][(long)b - (long)A] = 1.0;
          break;

          case 'O':
            for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
              ((dna_node*)nodep[i])->x[k][l][(long)b - (long)A] = 1.0;
            break;

          case '-':
            for (b = A; (long)b <= (long)T; b = (bases)((long)b + 1))
              ((dna_node*)nodep[i])->x[k][l][(long)b - (long)A] = 1.0;
            break;
        }
      }
    }
  }
}  /* makevalues2 */


void prot_freex_notip(long nonodes, pointarray treenode)
{
  /* used in proml */
  long i, j;
  node *p;

  for (i = spp; i < nonodes; i++)
  {
    p = treenode[i];
    if ( p == NULL ) continue;
    do {
      for (j = 0; j < endsite; j++)
      {
        free(((prot_node*)p)->x[j]);
        ((prot_node*)p)->x[j] = NULL;
      }
      free(((prot_node*)p)->x);
      ((prot_node*)p)->x = NULL;
      p = p->next;
    } while (p != treenode[i]);
  }
}  /* prot_freex_notip */


void codon_freex_notip(long nonodes, pointarray treenode)
{
  /* used in proml */
  long i, j;
  node *p;

  for (i = spp; i < nonodes; i++)
  {
    p = treenode[i];
    if ( p == NULL ) continue;
    do
    {
      for (j = 0; j < endsite; j++)
      {
        free(((codon_node*)p)->codonx[j]);
        ((codon_node*)p)->codonx[j] = NULL;
      }
      free(((codon_node*)p)->codonx);
      ((codon_node*)p)->codonx = NULL;
      p = p->next;
    } while (p != treenode[i]);
  }
}  /* codon_freex_notip */


void freex_notip(long nonodes, pointarray treenode)
{
  /* used in dnaml & dnamlk */
  long i, j;
  node *p;

  for (i = spp; i < nonodes; i++)
  {
    p = treenode[i];
    if ( p == NULL ) continue;
    do
    {
      for (j = 0; j < endsite; j++)
        free(((dna_node*)p)->x[j]);
      free(((dna_node*)p)->x);
      p = p->next;
    } while (p != treenode[i]);
  }
}  /* freex_notip */


void freex(long nonodes, pointarray treenode)
{
  /* used in dnaml & dnamlk */
  long i, j;
  node *p;

  for (i = 0; i < spp; i++)
  {
    for (j = 0; j < endsite; j++)
      free(((dna_node*)treenode[i])->x[j]);
    free(((dna_node*)treenode[i])->x);
  }

  for (i = spp; i < nonodes; i++)
  {
    if(treenode[i])
    {
      p = treenode[i];
      do {
        for (j = 0; j < endsite; j++)
          free(((dna_node*)p)->x[j]);
        free(((dna_node*)p)->x);
        p = p->next;
      } while (p != treenode[i]);
    }
  }
}  /* freex */


void update(tree *t, node *p)
{
  node *sib_ptr;

  if (!p->tip && !p->initialized)
    t->nuview((tree*)t, p);

  if ( p->back && !p->back->tip && !p->back->initialized)
    t->nuview((tree*)t, p->back);

  if ((!usertree) || (usertree && !lngths) || p->iter)
  {
    ((ml_tree*)t)->makenewv((tree*)t, p);

    if ( smoothit )
    {
      inittrav(p);
      inittrav(p->back);
    }
    else
    {
      // RSGdebug: Is this a model for fixing bogus BACK/NEXT pointers?
      if ( inserting )
      {
        if (!p->tip)
        {
          for ( sib_ptr = p->next;  sib_ptr != p ; sib_ptr = sib_ptr->next)
          {
            sib_ptr->initialized = false;
          }
        }
      }
    }
  }
}  /* update */


void smooth(tree* t, node *p)
{
  node *sib_ptr;

  if ( p == NULL )
    return;
  smoothed = false;

  update(t, p);

  if ( p->tip )
    return;
  if ( (smoothed && !polishing) || !smoothit )
    return;

  for ( sib_ptr = p->next ; sib_ptr != p ; sib_ptr = sib_ptr->next )
  {
    if ( sib_ptr->back )
    {
      smooth(t, sib_ptr->back);
      p->initialized = false;
      sib_ptr->initialized = false;
    }
  }
}  /* smooth */


static void ml_tree_smoothall(tree* t, node* p)
{
  boolean save;
  int i;
  node* q;

  save = smoothit;
  smoothit = true;
  if ( p->tip ) p = p->back;

  /* it may seem like we are doing too many smooths, but sometimes
   * one branch near p may already be completly smoothed from an
   * insert, this insures we spread out in the tree */
  for ( i = 0 ; i < smoothings ; i++ )
  {
    smooth(t, p->back);
    if ( p->tip ) return;
    for ( q = p->next ; q != p ; q = q->next)
      smooth(t, q->back);
  }
  smoothit = save;
} /* ml_tree_smoothall */


void ml_tree_do_branchl_on_insert(tree * t, node * forknode, node * q)
{ /* split q->v branch length evenly beween forknode->next and forknode->next->next */

  double newv;
  (void)t;                              // RSGnote: Parameter never used.

#if 0
  // This function currently does nothing; must be for future use.
  generic_do_branchl_on_insert(t, forknode, q);
#endif

  newv = q->v * 0.5;

  /*
   * forknode should be where node was inserted
   * set to initial v for *both* directions
   */
  forknode->v = initialv;
  forknode->back->v = initialv;

  /* forknode->next for both directions */
  forknode->next->v = newv ;
  forknode->next->back->v = newv ;

  /* forknode->next->next for both directions */
  forknode->next->next->v = newv;
  forknode->next->next->back->v = newv;

  /* BUG.970 -- might consider invalidating views here or in generic */
  inittrav(forknode);
  inittrav(forknode->back);
  inittrav(forknode->next);
  inittrav(forknode->next->back);
  inittrav(forknode->next->next);
  inittrav(forknode->next->next->back);
} /* ml_tree_do_branchl_on_insert */



void ml_tree_insert_(tree * t, node * p, node * q, boolean dooinit, boolean multf)
{
 /* 
  * After inserting via generic_, branch length gets initialv. If dooinit is
  * given, all branches are optimized, otherwise just those nearby.
  *
  * Insert q near p */
  long i;
  node * r;

  (void)multf;                          // RSGnote: Parameter never used.

  generic_tree_insert_(t, p, q, dooinit, false, 0); /* no multifurcate on ml insert_ */

  if ( !dooinit )
  {
    inserting = true;
    update(t, p->back);
    update(t, p->back->next);
    update(t, p->back->next->next);
    inserting = false;
  }
  else
  {
    for ( i = 0 ; i < smoothings ; i++)
    {
      smooth(t, p->back);
      for ( r = p->back->next ; r != p->back ; r = r->next )
        smooth(t, r);
    }
  }
} /* ml_tree_insert */


void ml_tree_do_branchl_on_re_move(tree* t, node* p, node*q)
{
  /* only works for bifurcations */
  /*
   * BUG.970 -- add this when moved into re_move
   * assert(q->next->next->next == q);
   *
   * also, should we call generic_do_branchl_on_re_move(t, p, q); ??
   */

  (void)t;                              // RSGnote: Parameter never used.
  (void)p;                              // RSGnote: Parameter never used.

  double combinedEdgeWeight = q->v + q->back->v;
  q->v       = combinedEdgeWeight;
  q->back->v = combinedEdgeWeight;

  /* BUG.970.INIT -- might consider invalidating views here or in generic */
  inittrav(q);
  inittrav(q->back);
}


void ml_tree_re_move(tree *t, node *p, node **q, boolean doinit)
{
  /* remove p and record in q where it was */
  /* assumes bifurcations */
  long i;

  generic_tree_re_move(t, p, q, doinit);

  if ( doinit )
  {
    for (i = 0 ; i < smoothings ; i++ )
    {
      smooth(t, *q);
      if ( smoothit )
        smooth(t, (*q)->back);
    }
  }
  else
    update(t, *q);
}


static boolean ml_tree_try_insert_thorough(tree* t, node* p, node* q, node **qwherein, double* bestyet, tree* bestree, tree* priortree)
{
  double like;
  boolean succeeded = false;

  t->insert_(t, p, q, true, false);

  like = t->evaluate(t, p, false);
  if (like > *bestyet || *bestyet == UNDEFINED)
  {
    *bestyet = like;
    t->copy(t, bestree);
    bestree->score = like;              /* This shouldn't be necessary */
    if ( qwherein != NULL )
      *qwherein = q;
    succeeded = true;
  }
  priortree->copy(priortree, t);

  return succeeded;
}


/* ml_tree_try_insert_
 *
 * Passes to ml_tree_try_insert_thorough or ml_tree_try_insert_notthorough
 * depending on the value of thorough. If multf is given, sets to
 * false.
 */

boolean ml_tree_try_insert_(tree* t, node* p, node* q, node **qwherein, double* bestyet, tree* bestree, tree* priortree, boolean thorough, boolean* multf)
{
  boolean succeeded;

  if ( multf )
    *multf = false;

  if ( thorough )
    succeeded = ml_tree_try_insert_thorough(t, p, q, qwherein, bestyet, bestree, priortree);
  else
    succeeded = ml_tree_try_insert_notthorough(t, p, q, qwherein, bestyet);

  return succeeded;
}


/* ml_tree_try_insert_notthorough
 *
 * Temporarily inserts p at q and evaluates. If the rearrangement is better than bestyet,
 * updates bestyet and returns true.
 */

static boolean ml_tree_try_insert_notthorough(tree *t, node *p, node *q, node** qwherein, double* bestyet)
{
  double like;
  boolean succeeded = false;

  t->save_traverses(t, p, q);
  t->insert_(t, p, q, false, false);
  like = t->evaluate(t, p, false);

  if (like > *bestyet || *bestyet == UNDEFINED)
  {
    *bestyet = like;
    *qwherein = q;
    succeeded = true;
  }
  node * whereRemoved;

  t->re_move(t, p, &whereRemoved, false); /* BUG.970 -- check doinit value */

  assert(whereRemoved == q);
  t->restore_traverses(t, p, q);

  /* Update t->score */
  t->evaluate(t, q, 0);

  return succeeded;
}


void mlk_tree_insert_(tree *t, node *newtip, node *below, boolean dummy, boolean dummy2, long k)
{
  /* inserts the nodes newfork and its descendant, newtip, into the tree. */
  long i;
  boolean done;
  node *p, *newfork;

  /* first stick it in the right place */
  rooted_tree_insert_(t, newtip, below, dummy, dummy2, k);

  below = t->nodep[below->index - 1];
  newfork = t->nodep[newtip->back->index - 1];
  newtip = t->nodep[newtip->index-1];
  /* now for the tyme stuff */
  if (((ml_node*)newtip)->node.tyme < ((ml_node*)below)->node.tyme)
    p = newtip;
  else p = below;

  set_tyme(newfork, ((ml_node*)p)->node.tyme);
  if (newfork->back != NULL)
  {
    /* here we rescale the tree to fit the subtree being added      *
     * note that if we are sticking a new node into the tree and    *
     * the branches are only epsilon appart, allow the branch       *
     * lengths to be 1/2 epsilon, so that we interfere with the     *
     * tree minimally                                               */
    if (((ml_node*)p)->node.tyme > ((ml_node*)newfork->back)->node.tyme)
      set_tyme(newfork, (((ml_node*)p)->node.tyme + ((ml_node*)newfork->back)->node.tyme) / 2.0);
    else
      set_tyme(newfork, ((ml_node*)p)->node.tyme - (epsilon/2));
    do
    {
      p = t->nodep[p->back->index - 1];
      done = (p == t->root);
      if (!done)
        done = (((ml_node*)t->nodep[p->back->index - 1])->node.tyme < ((ml_node*)p)->node.tyme);
      if (!done)
      {
        set_tyme(p->back, ((ml_node*)p)->node.tyme - epsilon/2);
      }
    } while (!done);
  }
  else
    set_tyme(newfork, ((ml_node*)newfork)->node.tyme - initialv);

  if ( !smoothit )
    smooth(t, newfork);
  else
  {
    inittrav(newtip);
    inittrav(newtip->back);
    for (i = 0 ; i < smoothings ; i++)
    {
      smooth(t, newfork);
      smooth(t, newfork->back);
    }
  }
}  /* mlk_tree_insert_ */


double get_tyme(node *p)
{ /* Return the tyme of an ml_node. p must point to struct ml_node. */
  return ((ml_node *)p)->node.tyme;
}


void set_tyme (node* p, double tyme)
{ /* Set the tyme of a node and its sibs. p must point to struct ml_node. */
  node *sib_ptr;
  sib_ptr = p;
  if ( p->next )
    do {
      ((ml_node*)sib_ptr)->node.tyme = tyme;
      /* added because changing tymes usually invalidates data likelihood.
       * This set seems to fix a failure to find the best tree in some
       * cases, but if the flags are being properly maintained it shouldn't...
       * apparent fix to bug#296, JY and MK 2015/05/18 */
      ((ml_node*)sib_ptr)->node.initialized = false;
      sib_ptr = sib_ptr->next;
    } while (sib_ptr != p );
  else
    ((ml_node*)p)->node.tyme = tyme;
}


void mlk_tree_re_move(tree* t, node *item, node** where, boolean doinit)
{
  // RSGnote: Originally the word "where" was the word "fork" in this comment, but that makes
  // no sense, as there is no variable "fork".  I *think* that the variable "where" was intended.
  /* Removes nodes item and its ancestor, where, from the tree.
     The new descendant of where's ancestor is made to be where's second descendant (other than item).
     Also returns pointers to the deleted nodes, item and where, and records where they were deleted from. */
  long i;
  node* whereloc;

  rooted_tree_re_move(t, item, &whereloc, doinit);
  if ( where )  *where = whereloc;

  if ( doinit )
  {
    inittrav(whereloc);
    inittrav(whereloc->back);
    for ( i = 0 ;  i < smoothings ; i++)
    {
      smooth(t, whereloc);
      smooth(t, whereloc->back);
    }
  }
  else smooth(t, whereloc->back);
}  /* mlk_tree_re_move */


#ifdef USE_NEW_MAKENEWV

/******* PROPAGATED FROM 3.6 ************/

double min_child_tyme(node *p)
{
  /* Return the minimum tyme of all children. p must be a parent nodelet */
  double min;
  node *q;

  min = 1.0; /* Tymes are always nonpositive */

  for ( q = p->next; q != p; q = q->next )
  {
    if ( get_tyme(q->back) < min )
      min = get_tyme(q->back);
  }

  return min;
}


double parent_tyme(node *p)
{
  /* Return the tyme of the parent of node p. p must be a parent nodelet. */
  if ( p->back )
    return get_tyme(p->back);
  /* else */
  return MIN_ROOT_TYME;
}


boolean valid_tyme(tree *t, node *p, double tyme)
{
  /* Return true if tyme is a valid tyme to assign to node p. tyme must be
   * finite, not greater than any of p's children, and not less than p's
   * parent. Also, tip nodes can only be assigned 0. Otherwise false is
   * returned. */

  p = t->nodep[p->index - 1];

#ifdef __USE_C99        /* TODO Find a way to check without this. */
  if ( !isfinite(tyme) ) return false;
#endif
  if ( p->tip == true && tyme != 0.0 ) return false;
  if ( tyme > min_child_tyme(p) ) return false;
  if ( tyme < parent_tyme(p) ) return false;

  return true;
}


double set_tyme_evaluate(tree *t, node *p, double tyme)
{
  /* Change tyme of node p and return likelihood
   * Views should be invalidated and regenerated before calling
   * evaluate() anywhere else in the tree. */

  /* node *sib_ptr;
     long num_sibs, i; */

  assert( valid_tyme(t, p, tyme) );

  set_tyme(p, tyme);
  t->nuview(t, p);

  /* TODO Seems to work without this, but make sure: */
#if 0
  num_sibs = count_sibs(p);
  sib_ptr = p;
  for (i=0 ; i < num_sibs; i++)
  {
    sib_ptr = sib_ptr->next;
    nuview(sib_ptr);
  }
#endif

  return t->evaluate(t, p, false);
}


void mlk_tree_makenewv(tree* t, node *p)
{
  /* Improve a node tyme using Newton-Raphson
   *
   * Slope and curvature are estimated at the current point and used to
   * interpolate a new point. If the curvature is positive, the next point
   * the estimations are pushed uphill by a fraction of the total range.
   * If any interpolation fails to produce a better result, the result is
   * retracted by a given factor toward the original point and tested again.
   *
   * The function terminates when max_iterations have been performed, or
   * when the likelihood improvement for any iteration is less than epsilon,
   * or when a retraction fails. If the iterations are exhausted,
   * 'smoothed' is set false, indicating that further improvement may be
   * possible by additional calls to makenewv(). Otherwise 'smoothed' is left
   * untouched.
   *
   * Define MAKENEWV_DEBUG to get lots of junk printed to stdout.
   * Each loop prints a character, as follows:
   *   '/' ->   Positive curvature, positive slope, moved +
   *   '\' ->   Positive curvature, negative slope, moved -
   *   ')' ->   Negative curvature, moved +
   *   '(' ->   Negative curvature, moved -
   *   '<' ->   Retracting back by retract_factor
   *   'X' ->   Retraction failed, keeping current point
   */

  /* Tuning constants */
  const double likelihood_epsilon = epsilon/1000.0;
  /* Any change in likelihood less than this, and we're done. */
  const double tyme_delta = epsilon;            /* Small tyme difference used
                                                   to compute slope and
                                                   curvature. */
  const double min_tyme_delta = tyme_delta / 10.0; /* Absolute smallest tyme_delta */
  const double uphill_step_factor = 0.05;       /* Fraction of the current
                                                   branch length to move uphill
                                                   in positive curvature
                                                   regions. */
  const double retract_factor = 0.5;            /* This defines how far back we
                                                   go if the interpolated point
                                                   is lower than the original
                                                */
  const double min_tdelta = epsilon;            /* Minimum to which we will
                                                   retract before giving up.
                                                */
  const long max_iterations = 100;              /* Maximum iterations -
                                                   typically we stop much
                                                   sooner */

  double min_tyme, max_tyme;
  double current_tyme, new_tyme;
  double current_likelihood, new_likelihood;
  double x[3], lnl[3];                          /* tyme (x) and log likelihood
                                                   (lnl) points below, at, and
                                                   above the current tyme */
  double s21, s10, slope;
  double curv;
  double uphill_step;
  double tdelta;                /* interpolated point minus current point */
  long iteration;
  boolean done;
  long num_sibs, i;
  node *sib_ptr;

  if ( p->tip )
    return;                     /* Skip tips. */

  node *s = t->nodep[p->index - 1];

#ifdef MAKENEWV_DEBUG
  double start_tyme = get_tyme(s);
  double start_likelihood = t->score;
  long uphill_steps = 0;
#endif /* MAKENEWV_DEBUG */

  /* Tyme cannot be less than parent */
  if (s == t->root)
    min_tyme = MIN_ROOT_TYME;
  else
    min_tyme = get_tyme(s) + MIN_BRANCH_LENGTH;

  /* Tyme cannot be greater than any children */
  max_tyme = min_child_tyme(s) - MIN_BRANCH_LENGTH;

  /* Nothing to do if we can't move */
  if ( max_tyme < min_tyme + 2.0*min_tyme_delta )
  {
    done = true;
    return;
  }

  current_tyme = get_tyme(s);
  current_likelihood = t->evaluate(t, s, false);

  uphill_step = (max_tyme - min_tyme) * uphill_step_factor;

  done = false;
  for ( iteration = 0; iteration < max_iterations; iteration++)
  {
    /* Evaluate three points for interpolation */
    x[0] = current_tyme - tyme_delta;
    if ( x[0] < min_tyme )
      x[0] = min_tyme;
    x[2] = current_tyme + tyme_delta;
    if ( x[2] > max_tyme )
      x[2] = max_tyme;
    x[1] = (x[0] + x[2]) / 2.0;

    lnl[0] = set_tyme_evaluate(t, s, x[0]);
    lnl[1] = set_tyme_evaluate(t, s, x[1]);
    lnl[2] = set_tyme_evaluate(t, s, x[2]);

    /* Compute slopes */
    s21 = (lnl[2] - lnl[1]) / (x[2] - x[1]);
    s10 = (lnl[1] - lnl[0]) / (x[1] - x[0]);
    slope = s21 + s10 / 2.0;

    /* Compute curvature */
    curv = (s21 - s10) / ((x[2] - x[0]) / 2);

    if (curv >= 0.0)
    {
      /* In negative curvature regions, just move uphill by a
       * fraction of the current length */
      tdelta = copysign(uphill_step, slope);
#ifdef MAKENEWV_DEBUG
      uphill_steps++;
      if ( tdelta > 0 ) putchar('/');
      else putchar('\\');
#endif /* MAKENEWV_DEBUG */
    }
    else
    {
      /* Otherwise guess where slope is 0 */
      tdelta = -(slope / curv);
#ifdef MAKENEWV_DEBUG
      if ( tdelta > 0 ) putchar(')');
      else putchar('(');
#endif /* MAKENEWV_DEBUG */
    }

    new_tyme = current_tyme + tdelta;
    if ( new_tyme <= min_tyme )
    {
      new_tyme = min_tyme;
      tdelta = new_tyme - current_tyme;
    }
    else if ( new_tyme >= max_tyme )
    {
      new_tyme = max_tyme;
      tdelta = new_tyme - current_tyme;
    }

    new_likelihood = set_tyme_evaluate(t, s, new_tyme);

    while ( new_likelihood < current_likelihood )
    {
      /* If our estimate is worse, retract until we find a better one */
#ifdef MAKENEWV_DEBUG
      putchar('<');
#endif /* MAKENEWV_DEBUG */
      tdelta *= retract_factor;
      uphill_step *= retract_factor;

      if ( fabs(tdelta) < min_tdelta )
      {
        /* Keep the current point and quit */
        new_likelihood = set_tyme_evaluate(t, s, current_tyme);
        done = true;
#ifdef MAKENEWV_DEBUG
        putchar('X');
#endif /* MAKENEWV_DEBUG */
        break;
      }

      new_tyme = current_tyme + tdelta;
      new_likelihood = set_tyme_evaluate(t, s, new_tyme);
    }

    if ( new_likelihood - current_likelihood < likelihood_epsilon )
    {
      done = true;
    }

    current_likelihood = new_likelihood;
    if ( done ) break;
  }

  /* invalidate and regenerate views */
  num_sibs = count_sibs(s);
  sib_ptr = p;
  for ( i = 0 ; i < num_sibs; i++ )
  {
    sib_ptr = sib_ptr->next;
    inittrav (sib_ptr);
  }

  if ( !done ) smoothed = false;

#ifdef MAKENEWV_DEBUG
  fprintf(stdout, "\nmakenewv(): node %ld: %ld iterations (%f,%f) => (%f,%f)\n", p->index, iteration+1, start_tyme, start_likelihood, current_tyme, current_likelihood);
#endif
}  /* mlk_tree_makenewv */


/******* END PROPAGATED FROM 3.6 ************/

#else /* ifndef USE_NEW_MAKENEWV */

void mlk_tree_makenewv(tree* t, node *p)
{
  /* improve a node time */
  long it, imin, imax, i;
  double tt, tfactor, tlow, thigh, oldlike, oldx, ymin, ymax, s32, s21, yold;
  boolean done, already;
  node *s, *sib_ptr, *sib_back_ptr;
  double tdelta, curv, slope, lnlike;
  double  x[3], lnl[3];

  /* don't do tips */
  if ( p->tip )
    return;

  s = t->nodep[p->index - 1];
  oldx = ((ml_node*)s)->node.tyme;                   /* store old tyme */
  lnlike = oldlike = t->evaluate(t, p, 0);           /* eval and store old likelihood */
  if (s == t->root)
    tlow = -10.0;                       /* default minimum tyme at root */
  else
    tlow = ((ml_node*)(s->back))->node.tyme; /* otherwise tyme >= parent tyme */

  /* set maximum tyme to smallest child tyme */
  sib_ptr = s;
  thigh = ((ml_node*)s->next->back)->node.tyme;
  for ( sib_ptr = s->next ; sib_ptr != s ; sib_ptr = sib_ptr->next )
  {
    sib_back_ptr = sib_ptr->back;
    if (((ml_node*)sib_back_ptr)->node.tyme < thigh)
      thigh = ((ml_node*)sib_back_ptr)->node.tyme;
  }
  /* nothing to be done if thigh and tlow are close to equal */
  if (thigh - tlow < 4.0*epsilon) return;

  if (s != t->root)
    tdelta = (thigh - tlow) / 10.0;
  else
  {
    tdelta = (thigh - ((ml_node*)s)->node.tyme) / 5.0;
    if (tdelta  < 2 * epsilon ) tdelta = 2 * epsilon;
  }

  getthree(t, s, thigh, tlow, tdelta, x, lnl); /* get three points for interpolation */

  it = 0;
  tfactor = 1.0;
  done = false;
  while (it < iterations && !done)
  {
    ymax = lnl[0];
    imax = 0;
    for (i = 1; i <= 2; i++)            /* figure out which point has the largest */
    {
      if (lnl[i] > ymax)                /* score */
      {
        ymax = lnl[i];
        imax = i;
      }
    }
    if (imax != 1)                      /* swap points so that x[1] scores highest */
    {
      /* TODO Explain why we are doing this */
      ymax = x[1];                      /* ymax is temporary only */
      x[1] = x[imax];
      x[imax] = ymax;
      ymax = lnl[1];
      lnl[1] = lnl[imax];
      lnl[imax] = ymax;
    }
    tt = x[1];
    yold = tt;

    /* avg slope near (x[2]+x[1])/2 */
    s32 = (lnl[2] - lnl[1]) / (x[2] - x[1]);
    /* avg slope near (x[1]+x[0])/2 */
    s21 = (lnl[1] - lnl[0]) / (x[1] - x[0]);

    if (fabs(x[2] - x[0]) > epsilon)
      /* avg curvature near (x[2]+2x[1]+x[0])/4 */
      curv = (s32 - s21) / ((x[2] - x[0]) / 2);
    else
      curv = 0.0;
    /* interpolate slope at x[1] */
    slope = (s32 + s21) / 2 - curv * (x[2] - 2 * x[1] + x[0]) / 4;
    if (curv >= 0.0)
    {
      if (slope < 0)
        tdelta = -fabs(tdelta);
      else
        tdelta = fabs(tdelta);
    }
    else
      tdelta = -(tfactor * slope / curv);
    if (tt + tdelta <= tlow + epsilon)
      tdelta = tlow + epsilon - tt;
    if (tt + tdelta >= thigh - epsilon)
      tdelta = thigh - epsilon - tt;
    tt += tdelta;
    done = (fabs(yold - tt) < epsilon || fabs(tdelta) < epsilon);
    set_tyme(s, tt);
    t->nuview(t, s);
    lnlike = t->evaluate(t, s, false);
    ymin = lnl[0];
    imin = 0;
    for (i = 1; i <= 2; i++)            /* figure out which of the three original */
    {
      if (lnl[i] < ymin)                /* points has the lowest ln score */
      {
        ymin = lnl[i];
        imin = i;
      }
    }
    already = (tt == x[0]) || (tt == x[1]) || (tt == x[2]);
    if (!already && ymin < lnlike)      /* if the minimum point is lower than   */
    {
      x[imin] = tt;                     /* our new interpolated point than take */
      lnl[imin] = lnlike;               /* that point and put it where the*/
    }                                   /* interpolated point is */
    if (already || lnlike < oldlike)
    {
      tt = oldx;                        /* if either our interpolated point has */
      set_tyme(s, oldx);                /* a lower score or is equivalent to    */
      tfactor /= 2;                     /* our original, reinterpolate this */
      tdelta /= 2;                      /* time go only half as far             */
      t->score = oldlike;
      lnlike = oldlike;
    }
    else
    {
      tfactor = 1.0;
      oldlike = lnlike;
      oldx = tt;
    }

    if (!done)                          /* apply it to the sibs */
    {
      set_tyme(p, tt);
      t->nuview(t, p);
      for (sib_ptr = p->next ; sib_ptr != p ; sib_ptr = sib_ptr->next )
        t->nuview(t, sib_ptr);
    }
    it++;
  }

  if ( smoothit )
    inittrav(p);
  p->initialized = false;
  for (sib_ptr = p->next ; sib_ptr != p ; sib_ptr = sib_ptr->next )
  {
    sib_ptr->initialized = false;
    if ( smoothit )
      inittrav(sib_ptr);
  }
  t->score = lnlike;
  smoothed = smoothed && done;
}  /* mlk_tree_makenewv */

#endif /* USE_NEW_MAKENEWV */


void getthree(tree* t, node *p, double thigh, double tlow, double tdelta, double *x, double *lnl)
{
  /* compute score at a new triple of points */
  int i;
  double tt = ((ml_node*)p)->node.tyme;
  double td = fabs(tdelta);

  x[0] = tt - td;
  x[1] = tt;
  x[2] = tt + td;

  if ( x[0] < tlow + epsilon )
  {
    x[0] = tlow + epsilon;
    x[1] = ( x[0] + x[2] ) / 2;
  }

  if ( x[2] > thigh - epsilon )
  {
    x[2] = thigh - epsilon;
    x[1] = ( x[0] + x[2] ) / 2;
  }

  for ( i = 0 ; i < 3 ; i++ )
  {
    set_tyme(p, x[i]);
    t->nuview(t, p);
    lnl[i] = t->evaluate(t, p, 0);
  }
}  /* getthree */


void empiricalfreqs(double *freqa, double *freqc, double *freqg, double *freqt, steptr weight, pointarray treenode)
{
  /* Get empirical base frequencies from the data */
  /* used in dnaml & dnamlk */
  /* this is kind of strange */
  long i, j, k;
  double sum, suma, sumc, sumg, sumt, w;

  *freqa = 0.25;
  *freqc = 0.25;
  *freqg = 0.25;
  *freqt = 0.25;

  for (k = 1; k <= 8; k++)
  {
    suma = 0.0;
    sumc = 0.0;
    sumg = 0.0;
    sumt = 0.0;

    for (i = 0; i < spp; i++)
    {
      for (j = 0; j < endsite; j++)
      {
        w = weight[j];
        sum = (*freqa) * ((dna_node*)treenode[i])->x[j][0][0];
        sum += (*freqc) * ((dna_node*)treenode[i])->x[j][0][(long)C - (long)A];
        sum += (*freqg) * ((dna_node*)treenode[i])->x[j][0][(long)G - (long)A];
        sum += (*freqt) * ((dna_node*)treenode[i])->x[j][0][(long)T - (long)A];
        suma += w * (*freqa) * ((dna_node*)treenode[i])->x[j][0][0] / sum;
        sumc += w * (*freqc) * ((dna_node*)treenode[i])->x[j][0][(long)C - (long)A] / sum;
        sumg += w * (*freqg) * ((dna_node*)treenode[i])->x[j][0][(long)G - (long)A] / sum;
        sumt += w * (*freqt) * ((dna_node*)treenode[i])->x[j][0][(long)T - (long)A] / sum;
      }
    }
    sum = suma + sumc + sumg + sumt;
    *freqa = suma / sum;
    *freqc = sumc / sum;
    *freqg = sumg / sum;
    *freqt = sumt / sum;
  }
  if (*freqa <= 0.0)
    *freqa = 0.000001;
  if (*freqc <= 0.0)
    *freqc = 0.000001;
  if (*freqg <= 0.0)
    *freqg = 0.000001;
  if (*freqt <= 0.0)
    *freqt = 0.000001;
}  /* empiricalfreqs */


void ml_treevaluate(tree* curtree, boolean improve, boolean reusertree, boolean global, boolean progress, tree* priortree, tree* bestree, inittravtree_t inittravtree)
{
  /* evaluate a user tree */

  smoothit = improve;
  if (reusertree)
  {
    arbitrary_resolve(curtree);
    curtree->smoothall(curtree, curtree->root);
    if (global)
      curtree->globrearrange(curtree, progress, smoothit);
    else
      curtree->locrearrange(curtree, curtree->root->back, smoothit, priortree, bestree);
    polishing = true;
    smoothit = true;
    curtree->smoothall(curtree, curtree->root);
    polishing = false;
  }
  else
  {
    inittravtree(curtree, curtree->root);
    polishing = true;
    smoothit = true;
    curtree->evaluate(curtree, curtree->root, 0);
    curtree->smoothall(curtree, curtree->root);
    curtree->smoothall(curtree, curtree->root);
    smoothit = improve;
    polishing= false;
  }
  curtree->evaluate(curtree, curtree->root, true);
}  /* ml_treevaluate */


void ml_inittravtree(tree* t, node *p)
{
  /* traverse tree to set initialized and v to initial values */
  node* q;

  p->initialized = false;
  p->back->initialized = false;

  if ((!lngths) || p->iter)
  {
    p->v = initialv;
    p->back->v = initialv;
  }

  if (!p->tip)
  {
    q = p->next;
    while ( q != p )
    {
      ml_inittravtree(t, q->back);
      q = q->next;
    }
  }
}  /* ml_inittravtree */


// End.
