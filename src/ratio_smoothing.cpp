/*
 * DisambigClusterSmoothing.cpp
 *
 *  Created on: Jan 25, 2011
 *      Author: ysun
 */

#include <limits>
#include <climits>
#include <sstream>
#include <cmath>
#include <algorithm>

#include <string.h>

#define HAVE_CPLEX 1
#if HAVE_CPLEX
#include <ilcplex/ilocplex.h>
#endif

#include "ratios.h"
#include "engine.h"


static const bool should_do_name_range_check = true;

void
smoothing_inter_extrapolation_cplex(map<SimilarityProfile, double,  SimilarityCompare> & ratio_map,
    const SimilarityProfile & min_sp,
    const SimilarityProfile & max_sp,
    const map<SimilarityProfile, uint32_t, SimilarityCompare> & x_counts,
    const map<SimilarityProfile, uint32_t, SimilarityCompare> & m_counts,
    const vector < string > & attribute_names,
    const bool name_range_check,
    const bool backup_quadprog);


uint32_t
sp2index ( const SimilarityProfile & sp, const SimilarityProfile & min_sp, const SimilarityProfile & max_sp ) {

    if ( sp.size() != min_sp.size() )
        throw cException_Other("Convertion error in smoothing.");
    uint32_t index = 0;
    for ( uint32_t i = 0; i < sp.size(); ++i ) {
        if ( sp.at(i) > max_sp.at(i) )
            throw cException_Other("Index range error. > max.");
        if ( sp.at(i) < min_sp.at(i) )
            throw cException_Other("Index range error. < min.");

        index *= max_sp.at(i) - min_sp.at(i) + 1;
        index += sp.at(i) - min_sp.at(i);
    }
    return index;
}


SimilarityProfile
index2sp (uint32_t index, const SimilarityProfile & min_sp,
    const SimilarityProfile & max_sp) {

    static const uint32_t ulimit = 0 - 1;
    SimilarityProfile sp (min_sp);

    for (uint32_t i = min_sp.size() - 1 ; i  != ulimit ; --i) {

        if (max_sp.at(i) < min_sp.at(i))
            throw cException_Other("Index range error.  max < min.");

        uint32_t t = max_sp.at(i) - min_sp.at(i) + 1 ;
        sp.at(i) += index % t;
        index /= t;
    }

    if (index >=1)
        throw cException_Other("Index range error.");

    return sp;
}


// This is probably dead code, but it's being called from something which
// is being linked, so we have to keep it in for now.
#if 1
void
cRatioComponent::smooth() {

    std::cout << "Starting data smoothing..." << std::endl;
    std::cout << "This step is skipped for cRatioComponent objects." << std::endl;
    return;

    map<SimilarityProfile, double, SimilarityCompare> temp_map = ratio_map;

    //smoothing( ratio_map, similarity_map, x_counts, m_counts, this->get_attrib_names(), should_do_name_range_check );
    const SimilarityProfile max = get_max_similarity (this->attrib_names);
    const SimilarityProfile min ( max.size(), 0);
    smoothing_inter_extrapolation_cplex(temp_map, min, max, x_counts, m_counts,
            this->get_attrib_names(), should_do_name_range_check, true);

    map<SimilarityProfile, double, SimilarityCompare>:: iterator p = ratio_map.begin();
    for (; p != ratio_map.end(); ++p) {
        p->second = temp_map.find(p->first)->second;
    }
    std::cout << "Smoothing done." << std::endl;
}
#endif



vector<SimilarityProfile>
find_lesser_neighbour(const SimilarityProfile & sp, const SimilarityProfile & min_sp) {

    vector<SimilarityProfile> vs;

    for (uint32_t i = 0; i < sp.size(); ++i) {

        if ( min_sp.at(i) > sp.at(i) )
            throw cException_Other("Similarity input error: sp < min.");
        else if ( min_sp.at(i) == sp.at(i))
            continue;
        else {
            SimilarityProfile temps (sp);
            temps.at(i) -= 1;
            vs.push_back(temps);
        }
    }
    return vs;
}


vector<SimilarityProfile>
find_greater_neighbour(const SimilarityProfile & sp, const SimilarityProfile & max_sp) {

    vector<SimilarityProfile> vs;

    for (uint32_t i = 0; i < sp.size(); ++i) {

        if ( max_sp.at(i) < sp.at(i) )
            throw cException_Other("Similarity input error: sp > max.");
        else if ( max_sp.at(i) == sp.at(i))
            continue;
        else {
            SimilarityProfile temps (sp);
            temps.at(i) += 1;
            vs.push_back(temps);
        }
    }
    return vs;
}


vector<std::pair<SimilarityProfile, SimilarityProfile> >
find_neighbours(const SimilarityProfile & sp,
                const SimilarityProfile & min_sp,
                const SimilarityProfile & max_sp) {

    vector<std::pair<SimilarityProfile, SimilarityProfile> > vs;

    for (uint32_t i = 0; i < sp.size(); ++i) {

        if (min_sp.at(i) > sp.at(i))
            throw cException_Other("Similarity input error: sp < min.");
        else if (max_sp.at(i) < sp.at(i))
            throw cException_Other("Similarity input error: sp > max.");
        else if (min_sp.at(i) == sp.at(i) || max_sp.at(i) == sp.at(i))
            continue;
        else {
            SimilarityProfile templess (sp), tempgreat(sp);
            templess.at(i) -= 1;
            tempgreat.at(i) += 1;
            vs.push_back(std::pair<SimilarityProfile, SimilarityProfile>(templess, tempgreat));
        }
    }
    return vs;
}


double
get_weight (const uint32_t x_count, const uint32_t m_count) {

    return 1.0 * x_count + 1.0 * m_count;
}


uint32_t
compute_total_nodes(const SimilarityProfile & min_sp,
                    const SimilarityProfile & max_sp) {

    uint32_t total_nodes = 1;
    // This is too clever and bad practice. STL provides
    // a maximum uint32, and that should be used in this
    // case. And yes, I know that the STL uses -1 to
    // signify maximum unsigned values, but we're not
    // writing STL code now, are we? That's right, we're
    // not.
    const uint32_t overflow_check = 0 - 1;

    for (uint32_t i = 0; i < min_sp.size(); ++i) {

        if (max_sp.at(i) < min_sp.at(i)) {
            throw cException_Other("Entry error: max < min.");
        }

        uint32_t t = max_sp.at(i) - min_sp.at(i) + 1;

        if (total_nodes >= overflow_check / t) {
            throw cException_Other ("Similarity profile count overflow).");
        } else {
            total_nodes *= t;
        }
    }

  return total_nodes;
}



void
smoothing_inter_extrapolation_cplex(
    SPRatiosIndex & ratio_map,
    const SimilarityProfile & min_sp,
    const SimilarityProfile & max_sp,
    const SPCountsIndex & x_counts,
    const SPCountsIndex & m_counts,
    const vector<string> & attribute_names,
    const bool name_range_check,
    const bool backup_quadprog ) {

    // TODO: Refactor x & m count checks
    if (x_counts.size() != m_counts.size()) {
        throw cException_Other("x_counts and m_counts are not of the same size");
    }

    if (x_counts.size() != ratio_map.size()) {
        throw cException_Other("x_counts and ratio_map are not of the same size");
    }

    // TODO: Refactor min and max sp size checks into the
    // compute_total_nodes function, then catch dissimilar
    // sizes in the unit testing.
    if (min_sp.size() != max_sp.size()) {
        throw cException_Other("Minimum & maximum similarity profile size difference.");
    }

    //first, build all the possible similarity profiles.
    // TODO: Refactor this into `compute_total_nodes`
#if 0
    uint32_t total_nodes = compute_total_nodes(min_sp, max_sp);
#else
    uint32_t total_nodes = 1;
    const uint32_t overflow_check = 0 - 1;

    for ( uint32_t i = 0; i < min_sp.size(); ++i ) {
        if ( max_sp.at(i) < min_sp.at(i) )
            throw cException_Other("Entry error: max < min.");
        uint32_t t = max_sp.at(i) - min_sp.at(i) + 1;
        if ( total_nodes >= overflow_check / t )
            throw cException_Other ("Size of all the similarity profiles exceeds the allowed limit ( uint32_t ).");
        else {
            total_nodes *= t;
        }
    }
#endif

    // TODO: Should be able to refactor this out.
    uint32_t total_possible_inequality = 0;
    uint32_t total_equality = 0;

    for (uint32_t i = 0; i < min_sp.size(); ++i) {
        uint32_t t = max_sp.at(i) - min_sp.at(i) + 1;
        uint32_t temp = total_nodes - total_nodes/t;

        if ( total_possible_inequality > overflow_check - temp )
            throw cException_Other ("Size of all inequality exceeds the allowed limit ( uint32_t ).");
        else {
            total_possible_inequality += temp;
            if ( temp ) {
                uint32_t temp2 = temp - total_nodes/t;
                total_equality += temp2;
            }
        }
    }

    std::cout << "There are " 
              << total_nodes << " similarity profiles, "
              << total_equality << " equalities and "
              << total_possible_inequality << " inequalities in all." << std::endl;
    ///// Refactor above^^^^^^

    std::cout << "Starting Quadratic Programming. ( Take the logarithm ) ..." << std::endl;

#if HAVE_CPLEX
    IloEnv env;
    const double xmin = log(1e-6);
    const double xmax = log(1e6);
    const double equ_delta = log (5);

    try {
        IloModel model(env);
        IloNumVarArray var(env);
        IloRangeArray con(env);

        //configuring variables
        for (uint32_t j = 0 ; j < total_nodes; ++j) {
            var.add (IloNumVar(env, xmin, xmax, ILOFLOAT));
        }

        //configuring the object
        IloExpr target(env);
        for ( map < SimilarityProfile, double,  SimilarityCompare  >::const_iterator cpm = ratio_map.begin();
                cpm != ratio_map.end(); ++cpm ) {

            const double log_val = log ( cpm->second);
            const double wt =  get_weight( x_counts.find (cpm->first)->second , m_counts.find (cpm->first)->second ) ;
            const uint32_t k = sp2index( cpm->first, min_sp, max_sp);
            const double quad_coef = wt;
            target += quad_coef * var[k] * var[k];
            const double line_coef = ( - 2.0 ) * log_val * wt;
            target += line_coef * var[k];
        }

        IloObjective obj ( env, target, IloObjective::Minimize);
        model.add(obj);

        //configuring constraints
        for ( uint32_t i = 0 ; i < total_nodes; ++i ) {
            const SimilarityProfile the_sp = index2sp(i, min_sp, max_sp );

            //equality constraints
            vector<std::pair<SimilarityProfile, SimilarityProfile> > neighbours
                = find_neighbours(the_sp, min_sp, max_sp);

            vector<std::pair<SimilarityProfile, SimilarityProfile> >::const_iterator ci = neighbours.begin();
            for (; ci != neighbours.end(); ++ci) {

                const uint32_t lesser = sp2index(ci->first, min_sp, max_sp);
                const uint32_t greater = sp2index (ci->second, min_sp, max_sp);

                if (lesser >= i)
                    throw cException_Other("Index sequence error. less > this.");

                if (greater <= i)
                    throw cException_Other("Index sequence error. greater < this.");

                IloRange rg (env, - equ_delta, 2.0 * var[i] - var[lesser] - var[greater] , equ_delta);
                con.add (rg);

            }

            //inequality constraints
            vector<SimilarityProfile> greater_neighbours (find_greater_neighbour (the_sp, max_sp));

            vector<SimilarityProfile>::const_iterator cc = greater_neighbours.begin();
            for (; cc != greater_neighbours.end(); ++cc) {
                const uint32_t gg = sp2index ( *cc, min_sp, max_sp );
                if (gg <= i)
                    throw cException_Other("Index sequence error. greater < this.");

                con.add ( var[gg] - var[i] >= 0);
            }
        }
        model.add(con);

        // solve the model
        IloCplex cpl(model);
        cpl.setOut(env.getNullStream());


        if ( ! cpl.solve() ) {
            env.error() << "Failed to optimize LP" << std::endl;
            throw(-1);
        }

        IloNumArray result(env);
        env.out() << "Solution status = " << cpl.getStatus() << std::endl;
        cpl.getValues(result, var);
        //cpl.exportModel("ratios.lp");

        //save the results

        for (uint32_t i = 0; i < total_nodes; ++i) {
            SimilarityProfile sp = index2sp(i, min_sp, max_sp);
            const double log_r = result[i];
            ratio_map[sp] = exp(log_r);
        }

        target.end();

    }
    catch (IloException& e) {
          std::cerr << "Concert exception caught: " << e << std::endl;
          throw;
    }
    catch ( int ) {
        throw;
    }
    env.end();
#endif
}


void
cRatios::smooth() {
    std::cout << "Starting ratios smoothing..." << std::endl;
    const SimilarityProfile max = get_max_similarity (this->attrib_names);
    const SimilarityProfile min ( max.size(), 0 );
    smoothing_inter_extrapolation_cplex(this->final_ratios, min, max, x_counts, m_counts,
            this->get_attrib_names(), should_do_name_range_check, false);

    std::cout << "Ratios smoothing done. " << std::endl;
}


