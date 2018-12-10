/* Derivation and subsumption rules for constraints.
   Copyright (C) 2013-2018 Free Software Foundation, Inc.
   Contributed by Andrew Sutton (andrew.n.sutton@gmail.com)

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "config.h"
#define INCLUDE_LIST
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "timevar.h"
#include "hash-set.h"
#include "machmode.h"
#include "vec.h"
#include "double-int.h"
#include "input.h"
#include "alias.h"
#include "symtab.h"
#include "wide-int.h"
#include "inchash.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "intl.h"
#include "flags.h"
#include "cp-tree.h"
#include "c-family/c-common.h"
#include "c-family/c-objc.h"
#include "cp-objcp-common.h"
#include "tree-inline.h"
#include "decl.h"
#include "toplev.h"
#include "type-utils.h"
#include "print-tree.h"

#if 0
static tree normalize_expression (tree, tree, tsubst_flags_t, tree);

static tree
normalize_connective (tree t, 
		      tree args, tree_code c, 
		      tsubst_flags_t complain, 
		      tree in_decl)
{
  tree t0 = normalize_expression (TREE_OPERAND (t, 0), args, complain, in_decl);
  tree t1 = normalize_expression (TREE_OPERAND (t, 1), args, complain, in_decl);
  return build_nt (c, t0, t1);
}

static tree
normalize_check (tree t, tree args, tsubst_flags_t complain, tree in_decl)
{
  tree tmpl = TREE_OPERAND (t, 0);
  tree result = tsubst_expr (t, args, complain, tmpl, false);
  if (result == error_mark_node)
    return error_mark_node;

  tree map = TREE_OPERAND (result, 1);
  
  // int n = TREE_VEC_LENGTH (args);
  // tree subst = make_tree_vec (n);
  // for (int i = 2; i <= n; ++i)
  //   SET_TMPL_ARGS_LEVEL (subst, i, TMPL_ARGS_LEVEL (mapping, i));
  // SET_TMPL_ARGS_LEVEL (subst, 1, args);

  // tree new_ag
  return t;
}

static tree
normalize_atom (tree t, tree args)
{
  tree c = build_nt (PRED_CONSTR, t);

  /* FIXME: The parameter mapping contains only arguments for parameters in
     the expression. This will yield false negatives. To implement this,
     just construct a list of template parameters in T. */
  TREE_TYPE (c) == args;
}

/* Return the normal form of T, having the given arguments.  */

static tree
normalize_expression (tree t, tree args, tsubst_flags_t complain, tree in_decl)
{
  tree_code c = TREE_CODE (t);
  switch (c)
    {
      case TRUTH_ANDIF_EXPR:
      case TRUTH_ORIF_EXPR:
        return normalize_connective (t, args, c, complain, in_decl);
      case TEMPLATE_ID_EXPR:
        return normalize_check (t, args, complain, in_decl);
      default: 
        normalize_atom (t, args);
    }
}
#endif

#if 0
namespace {

// Helper algorithms

template<typename I>
inline I
next (I iter)
{
  return ++iter;
}

template<typename I, typename P>
inline bool
any_p (I first, I last, P pred)
{
  while (first != last)
    {
      if (pred(*first))
        return true;
      ++first;
    }
  return false;
}

/* True if t is either and && or || expression.  */

bool
logical_connective_p (tree t)
{
  return TREE_CODE (t) == TRUTH_ANDIF_EXPR 
         || TREE_CODE (t) == TRUTH_ORIF_EXPR;
}

/* True if T does not correspond to a normalized term.  */

bool
non_normalized_p (tree t)
{
  return logical_connective_p (t) || concept_check_p (t);
}

bool prove_implication (tree, tree);

/*---------------------------------------------------------------------------
                           Proof state
---------------------------------------------------------------------------*/

struct term_entry
{
  tree t;
};

/* Hashing function and equality for hash term list entries.  The
   behavior depends on the kind of term. For expressions that would
   be normalized to atomic constraints, hashing and equality are based 
   on the identify of the expression and the value of their parameter 
   mapping. For concept checks, these functions are based on structural
   equivalence and their parameter mapping.  */

struct term_hasher : ggc_ptr_hash<term_entry>
{
  static hashval_t hash (term_entry *e)
  {
    return iterative_hash_template_arg (e->t, 0);
    // if (concept_check_p (e->t))
    //   return iterative_hash_template_arg (e->t, 0);
    // else
    //   return htab_hash_pointer (e->t);
  }

  static bool equal (term_entry *e1, term_entry *e2)
  {
    return cp_tree_equal (e1->t, e2->t);
    // if (TREE_CODE (e1->t) != TREE_CODE (e2->t))
    //   return false;
    // if (concept_check_p (e1->t))
    //   return cp_tree_equal (e1->t, e2->t);
    // return e1->t == e2->t;
  }
};

/* A term list is a list of atomic constraints. It is used
   to maintain the lists of assumptions and conclusions in a
   proof goal.

   Each term list maintains an iterator that refers to the current
   term. This can be used by various tactics to support iteration
   and stateful manipulation of the list. */
struct term_list
{
  typedef std::list<tree>::iterator iterator;

  term_list ();
  term_list (tree);

  bool includes (tree);
  iterator insert (iterator, tree);
  iterator push_back (tree);
  iterator erase (iterator);
  iterator replace (iterator, tree);
  iterator replace (iterator, tree, tree);

  iterator begin() { return seq.begin(); }
  iterator end() { return seq.end(); }

  std::list<tree>         seq;
  hash_table<term_hasher> tab;
};

inline
term_list::term_list ()
  : seq(), tab (11)
{
}

/* Initialize a term list with an initial term. */

inline
term_list::term_list (tree t)
  : seq (), tab (11)
{
  push_back (t);
}

/* Returns true if T is the in the tree. */

inline bool
term_list::includes (tree t)
{
  term_entry ent = {t};
  return tab.find (&ent);
}

/* Append a term to the list.  */

inline term_list::iterator
term_list::push_back (tree t)
{
  return insert (end(), t);
}

/* Insert a new (unseen) term T into the list before the proposition
   indicated by ITER. Returns the iterator to the newly inserted
   element.  */

term_list::iterator
term_list::insert (iterator iter, tree t)
{
  gcc_assert (!includes (t));
  iter = seq.insert (iter, t);
  term_entry ent = {t};
  term_entry** slot = tab.find_slot (&ent, INSERT);
  term_entry* ptr = ggc_alloc<term_entry> ();
  *ptr = ent;
  *slot = ptr;
  return iter;
}

/* Remove an existing term from the list. Returns an iterator referring
   to the element after the removed term.  */

term_list::iterator
term_list::erase (iterator iter)
{
  gcc_assert (includes (*iter));
  if (!logical_connective_p (*iter))
  {
    term_entry ent = {*iter};
    tab.remove_elt (&ent);
  }
  iter = seq.erase (iter);
  return iter;
}

/* Replace the given term with that specified. If the term has
   been previously seen, do not insert the term. Returns the
   first iterator past the current term.  */

term_list::iterator
term_list::replace (iterator iter, tree t)
{
  iter = erase (iter);
  if (!includes (t))
    insert (iter, t);
  return iter;
}


/* Replace the term at the given position by the supplied T1
   followed by t2. This is used in certain logical operators to
   load a list of assumptions or conclusions.  */

term_list::iterator
term_list::replace (iterator iter, tree t1, tree t2)
{
  iter = erase (iter);
  if (!includes (t1))
    insert (iter, t1);
  if (!includes (t2))
    insert (iter, t2);
  return iter;
}

/* A goal (or subgoal) models a sequent of the form
   'A |- C' where A and C are lists of assumptions and
   conclusions written as propositions in the constraint
   language (i.e., lists of trees). */

struct proof_goal
{
  term_list assumptions;
  term_list conclusions;
};

/* A proof state owns a list of goals and tracks the
   current sub-goal. The class also provides facilities
   for managing subgoals and constructing term lists. */

struct proof_state : std::list<proof_goal>
{
  proof_state ();

  iterator branch (iterator i);
  iterator discharge (iterator i);
};

/* Initialize the state with a single empty goal, and set that goal
   as the current subgoal.  */

inline
proof_state::proof_state ()
  : std::list<proof_goal> (1)
{ }

/* Branch the current goal by creating a new subgoal, returning a
   reference to the new object. This does not update the current goal. */

inline proof_state::iterator
proof_state::branch (iterator i)
{
  gcc_assert (i != end());
  proof_goal& g = *i;
  return insert (++i, g);
}

/* Discharge the current goal, setting it equal to the
   next non-satisfied goal. */

inline proof_state::iterator
proof_state::discharge (iterator i)
{
  gcc_assert (i != end());
  return erase (i);
}

/*---------------------------------------------------------------------------
                        Debugging
---------------------------------------------------------------------------*/

void
debug (term_list& ts)
{
  for (term_list::iterator i = ts.begin(); i != ts.end(); ++i)
    verbatim ("  # %E", *i);
}

void
debug (proof_goal& g, const char* label = "")
{
  verbatim ("-- %s --", label);
  debug (g.assumptions);
  verbatim ("       |-");
  debug (g.conclusions);
  verbatim ("=========");
}

/*---------------------------------------------------------------------------
                        Atomicity of constraints
---------------------------------------------------------------------------*/

/* Returns true if T is not an atomic constraint.  */

bool
non_atomic_constraint_p (tree t)
{
  if (TREE_CODE (t) == TRUTH_ANDIF_EXPR || TREE_CODE (t) == TRUTH_ORIF_EXPR)
    return true;
  return false;
}

/* Returns true if any constraints in T are not atomic.  */

bool
any_non_atomic_constraints_p (term_list& t)
{
  return any_p (t.begin(), t.end(), non_atomic_constraint_p);
}

/*---------------------------------------------------------------------------
                           Proof validations
---------------------------------------------------------------------------*/

enum proof_result
{
  invalid,
  valid,
  undecided
};

proof_result check_term (term_list&, tree);


proof_result
analyze_atom (term_list& ts, tree t)
{
  /* FIXME: Hook into special cases, if any. */
  /*
  term_list::iterator iter = ts.begin();
  term_list::iterator end = ts.end();
  while (iter != end)
    {
      ++iter;
    }
  */

  verbatim ("LAST RESORT %d %d", non_atomic_constraint_p (t), any_non_atomic_constraints_p (ts));

  if (non_atomic_constraint_p (t))
    return undecided;
  if (any_non_atomic_constraints_p (ts))
    return undecided;
  return invalid;
}

/* Search for a pack expansion in the list of assumptions that would
   make this expansion valid.  */

proof_result
analyze_pack (term_list& ts, tree t)
{
  tree c1 = PACK_EXPANSION_PATTERN (t);
  term_list::iterator iter = ts.begin();
  term_list::iterator end = ts.end();
  while (iter != end)
    {
      if (TREE_CODE (*iter) == TREE_CODE (t))
        {
          tree c2 = PACK_EXPANSION_PATTERN (*iter);
          if (prove_implication (c2, c1))
            return valid;
          else
            return invalid;
        }
      ++iter;
    }
  return invalid;
}

/* Search for concept checks in TS that we know subsume T. */

proof_result
search_known_subsumptions (term_list& ts, tree t)
{
  /*
  for (term_list::iterator i = ts.begin(); i != ts.end(); ++i)
    if (TREE_CODE (*i) == TEMPLATE_ID_EXPR)
      {
        if (bool* b = lookup_subsumption_result (*i, t))
          return *b ? valid : invalid;
      }
  */
  return undecided;
}

/* Determine if the terms in TS provide sufficient support for proving
   the proposition T. If any term in TS is a concept check that is known
   to subsume T, then the proof is valid. Otherwise, we have to expand T
   and continue searching for support.  */

proof_result
analyze_check (term_list& ts, tree t)
{
  proof_result r = search_known_subsumptions (ts, t);
  if (r != undecided)
    return r;

  tree tmpl = TREE_OPERAND (t, 0);
  tree args TREE_OPERAND (t, 1);
  tree c = expand_concept (tmpl, args);
  return check_term (ts, c);
}

/* The proof of a conjunction is valid iff both the LHS and 
   RHS are valid proofs.  */

proof_result
analyze_conjunction (term_list& ts, tree t)
{
  proof_result r = check_term (ts, TREE_OPERAND (t, 0));
  if (r == invalid || r == undecided)
    return r;
  return check_term (ts, TREE_OPERAND (t, 1));
}

/* The proof of a disjunction is valid iff either the LHS and
   rhs is a valid proof.  */

proof_result
analyze_disjunction (term_list& ts, tree t)
{
  proof_result r = check_term (ts, TREE_OPERAND (t, 0));
  if (r == valid)
    return r;
  return check_term (ts, TREE_OPERAND (t, 1));
}

proof_result
analyze_term (term_list& ts, tree t)
{
  switch (TREE_CODE (t))
    {
    case ERROR_MARK:
      return invalid;

    case TEMPLATE_ID_EXPR:
      return analyze_check (ts, t);

    case TRUTH_ANDIF_EXPR:
      return analyze_conjunction (ts, t);
    
    case TRUTH_ORIF_EXPR:
      return analyze_disjunction (ts, t);

    case EXPR_PACK_EXPANSION:
      return analyze_pack (ts, t);

    default:
      return analyze_atom (ts, t);
    }
}

/* Check if a single term can be proven from a set of assumptions.
   If the proof is not valid, then it is incomplete when either
   the given term is non-atomic or any term in the list of assumptions
   is not-atomic.  */

proof_result
check_term (term_list& ts, tree t)
{
  verbatim ("CHECK CONCLUSION %qE", t);
  for (tree a : ts)
    verbatim ("  FOR ASSUMPTION %qE", a);
  
  /* Try the easy way; search for an equivalent term.  */
  if (ts.includes (t)) {
    verbatim ("  PROVEN");
    return valid;
  }

  /* The hard way; actually consider what the term means.  */
  return analyze_term (ts, t);
}

/* Check to see if any term is proven by the assumptions in the
   proof goal. The proof is valid if the proof of any term is valid.
   If validity cannot be determined, but any particular
   check was undecided, then this goal is undecided.  */

proof_result
check_goal (proof_goal& g)
{
  term_list::iterator iter = g.conclusions.begin ();
  term_list::iterator end = g.conclusions.end ();
  bool incomplete = false;
  while (iter != end)
    {
      proof_result r = check_term (g.assumptions, *iter);
      if (r == valid)
        return r;
      if (r == undecided)
        incomplete = true;
      ++iter;
    }

    /* Was the proof complete? */
    if (incomplete)
      return undecided;
    else
      return invalid;
}

/* Check if the the proof is valid. This is the case when all
   goals can be discharged. If any goal is invalid, then the
   entire proof is invalid. Otherwise, the proof is undecided.  */

proof_result
check_proof (proof_state& p)
{
  proof_state::iterator iter = p.begin();
  proof_state::iterator end = p.end();
  while (iter != end)
    {
      proof_result r = check_goal (*iter);
      if (r == invalid)
        return r;
      if (r == valid)
        iter = p.discharge (iter);
      else
        ++iter;
    }

  /* If all goals are discharged, then the proof is valid.  */
  if (p.empty())
    return valid;
  else
    return undecided;
}

/*---------------------------------------------------------------------------
                           Left logical rules
---------------------------------------------------------------------------*/

term_list::iterator
load_check_assumption (term_list& ts, term_list::iterator i)
{
  tree tmpl = TREE_OPERAND (*i, 0);
  tree args = TREE_OPERAND (*i, 1);
  gcc_assert (concept_definition_p (tmpl));
  return ts.replace(i, expand_concept (tmpl, args));
}

term_list::iterator
load_parameterized_assumption (term_list& ts, term_list::iterator i)
{
  return ts.replace(i, PARM_CONSTR_OPERAND(*i));
}

term_list::iterator
load_conjunction_assumption (term_list& ts, term_list::iterator i)
{
  for (auto j = ts.begin(); j != ts.end(); ++j)
    verbatim ("HERE %qE %d", *j, j == i);
  tree t1 = TREE_OPERAND (*i, 0);
  tree t2 = TREE_OPERAND (*i, 1);
  return ts.replace(i, t1, t2);
}

/* Examine the terms in the list, and apply left-logical rules to move
   terms into the set of assumptions. */

void
load_assumptions (proof_goal& g)
{
  term_list::iterator iter = g.assumptions.begin();
  term_list::iterator end = g.assumptions.end();
  while (iter != end)
    {
      switch (TREE_CODE (*iter))
        {
        case TEMPLATE_ID_EXPR:
          iter = load_check_assumption (g.assumptions, iter);
          break;
        case TRUTH_ANDIF_EXPR:
          iter = load_conjunction_assumption (g.assumptions, iter);
          break;
        default:
          ++iter;
          break;
        }
    }
}

/* In each subgoal, load constraints into the assumption set.  */

void
load_assumptions(proof_state& p)
{
  proof_state::iterator iter = p.begin();
  while (iter != p.end())
    {
      load_assumptions (*iter);
      ++iter;
    }
}

void
explode_disjunction (proof_state& p, proof_state::iterator gi, term_list::iterator ti1)
{
  tree t1 = TREE_OPERAND (*ti1, 0);
  tree t2 = TREE_OPERAND (*ti1, 1);

  /* Erase the current term from the goal. */
  proof_goal& g1 = *gi;
  proof_goal& g2 = *p.branch (gi);

  /* Get an iterator to the equivalent position in th enew goal. */
  int n = std::distance (g1.assumptions.begin (), ti1);
  term_list::iterator ti2 = g2.assumptions.begin ();
  std::advance (ti2, n);

  /* Replace the disjunction in both branches. */
  g1.assumptions.replace (ti1, t1);
  g2.assumptions.replace (ti2, t2);
}


/* Search the assumptions of the goal for the first disjunction. */

bool
explode_goal (proof_state& p, proof_state::iterator gi)
{
  term_list& ts = gi->assumptions;
  term_list::iterator ti = ts.begin();
  term_list::iterator end = ts.end();
  while (ti != end)
    {
      if (TREE_CODE (*ti) == TRUTH_ORIF_EXPR)
        {
          explode_disjunction (p, gi, ti);
          return true;
        }
      else ++ti;
    }
  return false;
}

/* Search for the first goal with a disjunction, and then branch
   creating a clone of that subgoal. */

void
explode_assumptions (proof_state& p)
{
  proof_state::iterator iter = p.begin();
  proof_state::iterator end = p.end();
  while (iter != end)
    {
      if (explode_goal (p, iter))
        return;
      ++iter;
    }
}


/*---------------------------------------------------------------------------
                           Right logical rules
---------------------------------------------------------------------------*/

term_list::iterator
load_disjunction_conclusion (term_list& g, term_list::iterator i)
{
  tree t1 = TREE_OPERAND (*i, 0);
  tree t2 = TREE_OPERAND (*i, 1);
  return g.replace(i, t1, t2);
}

/* Apply logical rules to the right hand side. This will load the
   conclusion set with all tpp-level disjunctions.  */

void
load_conclusions (proof_goal& g)
{
  term_list::iterator iter = g.conclusions.begin();
  term_list::iterator end = g.conclusions.end();
  while (iter != end)
    {
      if (TREE_CODE (*iter) == TRUTH_ORIF_EXPR)
        iter = load_disjunction_conclusion (g.conclusions, iter);
      else
        ++iter;
    }
}

void
load_conclusions (proof_state& p)
{
  proof_state::iterator iter = p.begin();
  while (iter != p.end())
    {
      load_conclusions (*iter);
      ++iter;
    }
}

/*---------------------------------------------------------------------------
                          High-level proof tactics
---------------------------------------------------------------------------*/

/* Given two constraints A and C, try to derive a proof that
   A implies C.  */

bool
prove_implication (tree a, tree c)
{
  /* Quick accept. */
  if (cp_tree_equal (a, c))
    return true;

  /* Build the initial proof state. */
  proof_state proof;
  proof_goal& goal = proof.front();
  goal.assumptions.push_back(a);
  goal.conclusions.push_back(c);

  debug (goal, "initial");

  /* Perform an initial right-expansion in the off-chance that the right
     hand side contains disjunctions. */
  load_conclusions (proof);

  debug (goal, "before loop");

  int step_max = 1 << 10;
  int step_count = 0;              /* FIXME: We shouldn't have this. */
  std::size_t branch_limit = 1024; /* FIXME: This needs to be configurable. */
  while (step_count < step_max && proof.size() < branch_limit)
    {
      /* Determine if we can prove that the assumptions entail the
         conclusions. If so, we're done. */
      load_assumptions (proof);

      debug (proof.front(), "step");

      /* Can we solve the proof based on this? */
      proof_result r = check_proof (proof);
      if (r != undecided) {
        verbatim ("DONE %d", (int)r);
        return r == valid;
      }

      /* If not, then we need to dig into disjunctions.  */
      explode_assumptions (proof);

      ++step_count;
    }

  if (step_count == step_max)
    error ("subsumption failed to resolve");

  if (proof.size() == branch_limit)
    error ("exceeded maximum number of branches");

  return false;
}

} /* namespace */
#endif

static bool 
parameter_mapping_equivalent_p (tree t1, tree t2)
{
  tree map1 = TREE_TYPE (t1);
  tree map2 = TREE_TYPE (t2);
  while (map1 && map2)
    {
      tree arg1 = TREE_PURPOSE (map1);
      tree arg2 = TREE_PURPOSE (map2);
      if (!cp_tree_equal (arg1, arg2))
	return false;
      map1 = TREE_CHAIN (map1);
      map2 = TREE_CHAIN (map2);
    }
  return true;
}

/* 17.4.1.2p2. Two constraints are identical if they are formed
   from the same expression and the targets of the parameter mapping
   are equivalent.  */

static bool
constraint_identical_p (tree t1, tree t2)
{
  if (PRED_CONSTR_EXPR (t1) != PRED_CONSTR_EXPR (t2))
    return false;

  if (!parameter_mapping_equivalent_p (t1, t2))
    return false;
  
  return true;
}

static hashval_t
hash_atomic_constraint (tree t)
{
  /* Hash the identity of the expression.  */
  hashval_t val = htab_hash_pointer (PRED_CONSTR_EXPR (t));
    
  /* Hash the targets of the parameter map.  */
  tree p = TREE_TYPE (t);
  while (p)
    {
      val = iterative_hash_template_arg (TREE_PURPOSE (p), val);
      p = TREE_CHAIN (p);
    }

  return val;
}

/* Hash functions for atomic constrains.  */

struct constraint_hash : default_hash_traits<tree>
{
  static hashval_t hash (tree t)
  {
    return hash_atomic_constraint (t);
  }

  static bool equal (tree t1, tree t2)
  {
    return constraint_identical_p (t1, t2);
  }
};


/* A conjunctive or disjunctive clause.

   Each clause maintains an iterator that refers to the current
   term, which is used in the linear decomposition of a formula
   into CNF or DNF.  */

struct clause
{
  using iterator = std::list<tree>::iterator;
  using const_iterator = std::list<tree>::const_iterator;

  /* Initialize a clause with an initial term.  */

  clause (tree t)
  {
    m_terms.push_back (t);
    if (TREE_CODE (t) == PRED_CONSTR)
      m_set.add (t);

    m_current = m_terms.begin();
  }

  /* Create a copy of the current term. The current
     iterator is set to point to the same position in the
     copied list of terms.  */

  clause (clause const& c)
    : m_terms (c.m_terms), m_current (m_terms.begin ())
  {
    std::advance (m_current, std::distance (c.begin (), c.current ()));
  }

  /* Returns true when all terms are atoms.  */

  bool done() const
  {
    return m_current == end ();
  }

  /* Advance to the next term.  */

  void advance ()
  {
    gcc_assert (!done ());
    ++m_current;
  }

  /* Replaces the current term at position ITER with T.  If
     T is an atomic constraint that already appears in the
     clause, remove but do not replace ITER. Returns a pair
     containing an iterator to the replace object or past
     the erased object and a boolean value which is true if
     an object was erased.  */

  std::pair<iterator, bool> replace (iterator iter, tree t)
  {
    gcc_assert (TREE_CODE (*iter) != PRED_CONSTR);
    if (TREE_CODE (t) == PRED_CONSTR)
      {
	if (m_set.add (t))
	  return std::make_pair (m_terms.erase(iter), true);
      }
    *iter = t;
    return std::make_pair (iter, false);
  }

  /* Inserts T before ITER in the list of terms.  If T has 
     already is an atomic constraint that already appears in
     the clause, no action is taken, and the current iterator
     is returned. Returns a pair of an iterator to the inserted
     object or ITER if no insertion occurred and a boolean
     value which is true if an object was inserted.  */

  std::pair<iterator, bool> insert (iterator iter, tree t)
  {
    if (TREE_CODE (t) == PRED_CONSTR)
    {
      if (m_set.add (t))
      	return std::make_pair (iter, false);
    }
    return std::make_pair (m_terms.insert (iter, t), true);
  }

  /* Replaces the current term with T. In the case where the
     current term is erased (because T is redundant), update
     the position of the current term to the next term.  */
  
  void replace (tree t)
  {
    m_current = replace (m_current, t).first;
  }
  
  /* Replace the current term with T1 and T2, in that order.  */

  void replace (tree t1, tree t2)
  {
    /* Replace the current term with t1. Ensure that iter points 
       to the term before which t2 will be inserted.  Update the
       current term as needed.  */
    std::pair<iterator, bool> rep = replace (m_current, t1);
    if (rep.second)
      m_current = rep.first;
    else
      ++rep.first;
    
    /* Insert the t2. Make this the current term if we erased
       the prior term.  */
    std::pair<iterator, bool> ins = insert (rep.first, t2);
    if (rep.second && ins.second)
      m_current = ins.first;
  }

  /* Returns true if the clause contains the term T.  */

  bool contains (tree t)
  {
    gcc_assert (TREE_CODE (t) == PRED_CONSTR);
    return m_set.contains (t);
    
    // for (const_iterator i = begin(); i != end(); ++i)
    //   if (constraint_identical_p (*i, t))
    //   	return true;
    // return false;
  }


  /* Returns an iterator to the first clause in the formula.  */

  iterator begin ()
  {
    return m_terms.begin();
  }

  /* Returns an iterator to the first clause in the formula.  */

  const_iterator begin () const
  {
    return m_terms.begin();
  }

  /* Returns an iterator past the last clause in the formula.  */

  iterator end ()
  {
    return m_terms.end();
  }

  /* Returns an iterator past the last clause in the formula.  */

  const_iterator end () const
  {
    return m_terms.end();
  }

  /* Returns the current iterator.  */

  const_iterator current () const
  {
    return m_current;
  }

  std::list<tree> m_terms; /* The list of terms.  */
  hash_set<tree, constraint_hash> m_set; /* The set of atomic constraints.  */
  iterator m_current; /* The current term.  */
};


/* A proof state owns a list of goals and tracks the
   current sub-goal. The class also provides facilities
   for managing subgoals and constructing term lists. */

struct formula
{
  using iterator = std::list<clause>::iterator;
  using const_iterator = std::list<clause>::const_iterator;

  /* Construct a formula with an initial formula in a
     single clause.  */

  formula (tree t)
  {
    m_clauses.emplace_back (t);
    m_current = m_clauses.begin ();
  }

  /* Returns true when all clauses are atomic.  */
  bool done () const
  {
    return m_current == end ();
  }

  /* Advance to the next term.  */
  void advance ()
  {
    gcc_assert (!done ());
    ++m_current;
  }

  /* Insert a copy of clause into the formula. This corresponds 
     to a distribution of one logical operation over the other.  */

  clause& branch ()
  {
    gcc_assert (!done ());
    m_clauses.push_back (*m_current);
    return m_clauses.back();
  }

  /* Returns the position of the current clause.  */

  iterator current ()
  {
    return m_current;
  }

  /* Returns an iterator to the first clause in the formula.  */

  iterator begin ()
  {
    return m_clauses.begin();
  }

  /* Returns an iterator to the first clause in the formula.  */

  const_iterator begin () const
  {
    return m_clauses.begin();
  }

  /* Returns an iterator past the last clause in the formula.  */

  iterator end ()
  {
    return m_clauses.end();
  }

  /* Returns an iterator past the last clause in the formula.  */

  const_iterator end () const
  {
    return m_clauses.end();
  }

  std::list<clause> m_clauses; /* The list of clauses.  */
  iterator m_current; /* The current clause.  */
};

void
debug (clause& c)
{
  for (clause::iterator i = c.begin(); i != c.end(); ++i)
    verbatim ("  # %E", *i);
}

void
debug (formula& f)
{
  for (formula::iterator i = f.begin(); i != f.end(); ++i)
    {
      verbatim ("(((");
      debug (*i);
      verbatim (")))");
    }
}

/* The logical rules used to analyze a logical formula. The
   "left" and "right" refer to the position of formula in a
   sequent (as in sequent calculus).  */

enum rules 
{
  left, right
};

/* Returns true if t distributes over its operands.  */

static bool
distributes_p (tree t)
{
  tree t1 = TREE_OPERAND (t, 0);
  tree t2 = TREE_OPERAND (t, 1);
  if (TREE_CODE (t) == CONJ_CONSTR)
    return TREE_CODE (t1) == DISJ_CONSTR && TREE_CODE (t2) == DISJ_CONSTR;
  if (TREE_CODE (t) == DISJ_CONSTR)
    return TREE_CODE (t1) == CONJ_CONSTR && TREE_CODE (t2) == CONJ_CONSTR;
  return false;
}

static int count_terms (tree, rules);

/* The maximum number of allowable terms in a constraint.  */

static int max_size = 4096;

/* Returns the sum of a and b. If the result would overflow,
   returns -1 to indicate an error condition.  */

static inline int
add_clamped (int a, int b)
{
  long long n = (long long)a + (long long)b;
  if (n > max_size)
    return -1;
  return n;
}

/* Returns the product of a and b. If the result would overflow,
   returns -1 to indicate an error condition.  */

static inline int
mul_clamped (int a, int b)
{
  long long n = (long long) a * (long long)b;
  if (n > max_size)
    return -1;
  return n;
}

/* Returns the number of clauses for a conjunction. When converting
   to DNF (when R == LEFT), a conjunction of disjunctions (i.e., 
   terms in CNF-like form) can grow exponentially.  */

int
count_conjunction (tree t, rules r)
{
  int n1 = count_terms (TREE_OPERAND (t, 0), r);
  int n2 = count_terms (TREE_OPERAND (t, 1), r);
  if (n1 == -1 || n2 == -1)
    return -1;
  if (r == left && distributes_p (t))
    return mul_clamped (n1, n2);
  return add_clamped (n1, n2);
}

/* Returns the number of clauses for a conjunction. When converting
   to CNF (when R == RIGHT), a disjunction of conjunctions (i.e., 
   terms in DNF-like form) can grow exponentially.  */

static int
count_disjunction (tree t, rules r)
{
  int n1 = count_terms (TREE_OPERAND (t, 0), r);
  int n2 = count_terms (TREE_OPERAND (t, 1), r);
  if (n1 == -1 || n2 == -1)
    return -1;
  if (r == right && distributes_p (t))
    return mul_clamped (n1, n2);
  return add_clamped (n1, n2);
}

/* Count the number of subproblems in T.  */

static int
count_terms (tree t, rules r)
{
  switch (TREE_CODE (t))
    {
    case CONJ_CONSTR:
      return count_conjunction (t, r);
    case DISJ_CONSTR:
      return count_disjunction (t, r);
    default:
      return 1;
    }
}

/* Returns the maximum number of terms in T if it were
   converted to DNF.  Returns -1 if the count exceeds the
   maximum formula size.  */

static int
dnf_size (tree t)
{
  return count_terms (t, left);
}

/* Returns the maximum number of terms in T if it were 
   converted to CNF.  Returns -1 if the count exceeds the
   maximum formula size.  */

static int
cnf_size (tree t)
{
  return count_terms (t, right);
}

/* A left-conjunction is replaced by its operands.  */

void
replace_term (clause& c, tree t)
{
  tree t1 = TREE_OPERAND (t, 0);
  tree t2 = TREE_OPERAND (t, 1);
  return c.replace(t1, t2);
}

/* Create a new clause in the formula by copying the current
   clause. In the current clause, the term at CI is replaced 
   by the first operand, and in the new clause, it is replaced 
   by the second.  */

void
branch_clause (formula& f, clause& c1, tree t)
{
  tree t1 = TREE_OPERAND (t, 0);
  tree t2 = TREE_OPERAND (t, 1);
  clause& c2 = f.branch ();
  c1.replace (t1);
  c2.replace (t2);
}

/* Decompose t1 /\ t2 according to the rules R.  */

inline void
decompose_conjuntion (formula& f, clause& c, tree t, rules r)
{
  if (r == left)
    replace_term (c, t);
  else
    branch_clause (f, c, t);
}

/* Decompose t1 \/ t2 according to the rules R.  */

inline void
decompose_disjunction (formula& f, clause& c, tree t, rules r)
{
  if (r == right)
    replace_term (c, t);
  else
    branch_clause (f, c, t);
}

/* An atomic constraint is already decomposed.  */
inline void
decompose_atom (clause& c)
{
  c.advance ();
}

/* Decompose a term of clause C (in formula F) according to the
   logical rules R. */

void
decompose_term (formula& f, clause& c, tree t, rules r)
{
  switch (TREE_CODE (t))
    {
      case CONJ_CONSTR:
        return decompose_conjuntion (f, c, t, r);
      case DISJ_CONSTR:
	return decompose_disjunction (f, c, t, r);
      default:
	return decompose_atom (c);
    }
}

/* Decompose C (in F) using the logical rules R until it 
   is comprised of only atomic constraints.  */

void
decompose_clause (formula& f, clause& c, rules r)
{
  while (!c.done ())
    decompose_term (f, c, *c.current (), r);
  f.advance ();
}

/* Decompose the logical formula F according to the logical
   rules determined by R.  The result is a formula containing
   clauses that contain only atomic terms.  */

void
decompose_formula (formula& f, rules r)
{
  while (!f.done ())
    decompose_clause (f, *f.current (), r);
}

/* Fully decomposing T into a list of sequents, each comprised of
   a list of atomic constraints, as if T were an antecedent.  */

static formula 
decompose_antecedent (tree t)
{
  formula f (t);
  decompose_formula (f, left);
  return f;
}

/* Fully decomposing T into a list of sequents, each comprised of
   a list of atomic constraints, as if T were a consequent.  */

static formula
decompose_consequents (tree t)
{
  formula f (t);
  decompose_formula (f, right);
  return f;
}

static bool derive_proof (clause&, tree, rules);

/* Derive a proof of both operands of T.  */

static bool
derive_proof_for_both_operands (clause& c, tree t, rules r)
{
  if (!derive_proof (c, TREE_OPERAND (t, 0), r))
    return false;
  return derive_proof (c, TREE_OPERAND (t, 1), r);
}

/* Derive a proof of either operand of T.  */

static bool
derive_proof_for_either_operand (clause& c, tree t, rules r)
{
  if (derive_proof (c, TREE_OPERAND (t, 0), r))
    return true;
  return derive_proof (c, TREE_OPERAND (t, 1), r);
}

/* Derive a proof of the atomic constraint T in clause C.  */

static bool
derive_atomic_proof (clause& c, tree t)
{
  return c.contains (t);
}

/* Derive a proof of T from the terms in C.  */

static bool
derive_proof (clause& c, tree t, rules r)
{
  switch (TREE_CODE (t))
  {
    case CONJ_CONSTR:
      if (r == left)
        return derive_proof_for_both_operands (c, t, r);
      else
      	return derive_proof_for_either_operand (c, t, r);
    case DISJ_CONSTR:
      if (r == left)
        return derive_proof_for_either_operand (c, t, r);
      else
      	return derive_proof_for_both_operands (c, t, r);
    default:
      return derive_atomic_proof (c, t);
  }
}

/* Derive a proof of T from disjunctive clauses in F.  */

static bool
derive_proofs (formula& f, tree t, rules r)
{
  for (formula::iterator i = f.begin(); i != f.end(); ++i)
    if (!derive_proof (*i, t, r))
      return false;
  return true;
}

static inline bool
diagnose_constraint_size (tree t)
{
  error_at (input_location, "%qE exceeds the maximum constraint complexity", t);
  return false;
}

/* Returns true if the LEFT constraint subsume the RIGHT constraints.
   This is done by deriving a proof of the conclusions on the RIGHT
   from the assumptions on the LEFT assumptions.  */

static bool
subsumes_constraints_nonnull (tree lhs, tree rhs)
{
  auto_timevar time (TV_CONSTRAINT_SUB);

  int n1 = dnf_size (lhs);
  int n2 = cnf_size (rhs);
  
  /* If either constraint would overflow complexity, bail.  */
  if (n1 == -1)
    return diagnose_constraint_size (lhs);
  if (n2 == -1)
    return diagnose_constraint_size (rhs);
  
  /* Decompose the smaller of the two formulas, and recursively
     check the implication using the larger.  */
  if (n1 <= n2)
    {
      formula dnf = decompose_antecedent (lhs);
      return derive_proofs (dnf, rhs, left);
    }
  else
    {
      formula cnf = decompose_consequents (rhs);
      return derive_proofs (cnf, lhs, right);
    }
}

/* Returns true if the LEFT constraints subsume the RIGHT
   constraints.  */

bool
subsumes (tree lhs, tree rhs)
{
  if (lhs == rhs)
    return true;
  if (!lhs)
    return false;
  if (!rhs)
    return true;
  return subsumes_constraints_nonnull (lhs, rhs);
}
