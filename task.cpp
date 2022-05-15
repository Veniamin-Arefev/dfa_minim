#include "api.hpp"
#include <string>
#include <queue>
#include <iostream>
#include <map>

DFA dfa_minim(DFA &d) {
    Alphabet alphabet = d.get_alphabet();
    std::string alphabet_string = alphabet.to_string();
    auto states = d.get_states();

    std::map<std::string, int> states_to_int;
    std::vector<std::string> int_to_states;
    if (true) {
        int i = 0;
        states_to_int["dead_state"] = i++;
        int_to_states.push_back("dead_state");
        for (auto &elem: d.get_states()) {
            states_to_int[elem] = i++;
            int_to_states.push_back(elem);
        }
    }

// part 1
    std::map<std::string, std::map<char, std::set<std::string>>> trans_reverse;
    for (auto &state: states) {
        for (auto &trans_sym: alphabet) {
            if (d.has_trans(state, trans_sym)) {
                trans_reverse[d.get_trans(state, trans_sym)][trans_sym].insert(state);
            } else {
                trans_reverse["dead_state"][trans_sym].insert(state);
            }
        }
    }
    for (auto &trans_sym: alphabet) {
        trans_reverse["dead_state"][trans_sym].insert("dead_state");
    }
    states.insert("dead_state");

// just print
//    if (true) {
//        int i = 0;
//        for (const auto &[i, elem]: states_to_int) {
//            std::cout << i << " is  " << elem << std::endl;
//        }
//    }
//
//    for (const auto &[key, value]: trans_reverse) {
//        std::cout << "to <" << key << ">" << std::endl;
//        for (const auto &[key1, value1]: value) {
//            std::cout << "   by " << key1 << " from ";
//            for (auto &state: value1) {
//                std::cout << state << ' ';
//            }
//            std::cout << std::endl;
//        }
//    }

    std::vector<std::vector<bool>> marked(states_to_int.size());
    for (int i = 0; i < states_to_int.size(); ++i) {
        marked[i].resize(states_to_int.size(), true);
    }
    std::queue<std::pair<std::string, std::string>> my_queue;

// part 2
    for (auto &final_state: d.get_final_states()) {
        for (auto &state: states) {
            if (d.get_final_states().count(state) != 1) {
                marked[states_to_int[final_state]][states_to_int[state]] = false;
                marked[states_to_int[state]][states_to_int[final_state]] = false;
//                std::cout << final_state << "  " << state << std::endl;
                my_queue.push(std::make_pair(final_state, state));
            }
        }
    }

//    std::map<std::string, std::map<char, std::set<std::string>>> trans_reverse;
    while (!my_queue.empty()) {
        for (auto &trans_sym: alphabet) {
            auto &state1 = my_queue.front().first;
            auto &state2 = my_queue.front().second;

            auto &new_states1 = trans_reverse[state1][trans_sym];
            auto &new_states2 = trans_reverse[state2][trans_sym];

            for (auto &new_state1: new_states1) {
                for (auto &new_state2: new_states2) {
                    if (marked[states_to_int[new_state1]][states_to_int[new_state2]]) {
                        marked[states_to_int[new_state1]][states_to_int[new_state2]] = false;
                        marked[states_to_int[new_state2]][states_to_int[new_state1]] = false;
//                        std::cout << "Added " << new_state1 << "  " << new_state2 << std::endl;
                        my_queue.push(std::make_pair(new_state1, new_state2));
                    }
                }
            }
        }
        my_queue.pop();
    }

    std::set<std::set<int>> new_states_set;
    for (int i = 0; i < marked.size(); ++i) {
        std::set<int> new_state;
        for (int j = 0; j < marked.size(); ++j) {
            if (marked[i][j]) {
                new_state.insert(j);
            }
        }
        new_states_set.insert(new_state);
    }

    std::vector<std::set<int>> new_states;
    std::copy(new_states_set.begin(), new_states_set.end(), back_inserter(new_states));

    new_states.erase(new_states.begin());

//    for (auto &elems: new_states) {
//        for (auto &elem: elems) {
//            std::cout << int_to_states[elem] << ' ';
//        }
//        std::cout << std::endl;
//    }

    DFA dfa = DFA(alphabet);
    for (int i = 0; i < new_states.size(); ++i) {
        bool is_final = false;
        bool is_initial = false;
        for (auto &state_index: new_states[i]) {
            if (d.is_final(int_to_states[state_index])) {
                is_final = true;
            }
            if (d.is_initial(int_to_states[state_index])) {
                is_initial = true;
            }
        }
        dfa.create_state(std::to_string(i), is_final);
        if (is_initial) {
            dfa.set_initial(std::to_string(i));
        }
    }

    std::vector<int> old_state_to_new_state(states.size());
    for (int i = 0; i < new_states.size(); ++i) {
        for (auto &state_index: new_states[i]) {
            old_state_to_new_state[state_index] = i;
        }
    }


    //    std::map<std::string, std::map<char, std::set<std::string>>> trans_reverse;
    for (int i = 0; i < new_states.size(); ++i) {
        for (auto &state_index: new_states[i]) {
            for (const auto &[trans_sym, states_from]: trans_reverse[int_to_states[state_index]]) {
//                bool set_trans(const std::string &from, char by, const std::string &to);
                for (auto &old_state: states_from) {
//                    std::cout << "from " << old_state << " to " << std::to_string(i) << std::endl;
                    dfa.set_trans(std::to_string(old_state_to_new_state[states_to_int[old_state]]),
                                  trans_sym,
                                  std::to_string(i));
                }
            }
        }
    }

    return dfa;
//    return d;
}
