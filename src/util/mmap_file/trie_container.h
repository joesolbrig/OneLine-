#ifndef TRIE_CONTAINER_H
#define TRIE_CONTAINER_H

using namespace std;
#include <vector>
#include <string>
#include <utility>


//This will search quickly only in terms of find ALL MATCHES
// BUT it won't find items SORTED by value...
// It makes sense WHEN AND ONLY WHEN we have a long string...
// OR not at all...

struct ItemRef{
    string name;
    long id;
    long level;

};


class TrieNode {
    private:
        hash_map< char, TrieEntry*> entries;
        map< level, TrieEntry*> entries_by_level;
        bool end_of_word;
        long id;
        long highest_contained_level;
    public:
        TrieNode(){ end_of_word=true;}
        TrieNode(string s){
            end_of_word=false;
            id = -1;
            highest_contained_level =-1;
        }
        void insert(string str, long objectId, long level){
            unsigned int s = str.size();
            highest_contained_level = MAX(highest_contained_level, level);
            if(s==0){
                end_of_word=true;
                id = objectId;
                return;
            }
            char c = str[0];
            if(!entries.contain(c)){
                entries[c] = new TrieNode();
            }
            long old_level =entries[c]->highest_contained_level;
            entries[c]->insert(str.substr(1, -1));
            if(old_level!=entries[c].highest_contained_level){
                entries_by_level[entries[c]->highest_contained_level] =
                        entries[c];
            }
        }

        Vector<ItemRef> find(string name, number_desired, Vector<ItemRef> v){
            if(name.empty()){
                //id will show if this is success
                v.push_back(Pair<string, long>("",id));
                return v;
            }
            char c = str[0];
            if(c=='?'){
                find_in_all_branchs(name, number_desired, v);
            }


        }

        ItemList find_in_all_branchs(string name, number_desired, ItemList v){
            if(end_of_word){
                v.push_back(Pair<string, long>("",id));
            }
            hash_map::iterator i = entries_by_level.begin();
            for(;i!=entries.begin();i++){
                ItemList sv = entries[i.value()].find(name.substr(1, -1));
                sv = prepend_chars(name[1], sv);
                if(!(sv.length ==1 and sv.second()==-1)){
                    v.push_back(sv);
                }

            }
            return v;
        }


        ItemList prepend_chars(char c, ItemList v){
            ItemList r;
            for(int i=0;i<v.size();i++){
                ItemRef t = v[i];
                t.name = c + t.name;
                r.push_back(t);
            }
            return r;
        }



}

#endif // TRIE_CONTAINER_H
