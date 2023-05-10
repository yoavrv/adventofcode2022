#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <functional>
#include <vector>
#include <ranges>
#include <algorithm>
#include <stdexcept>

using namespace std;

enum class optype {addition, multiplication, square};

class Monkey {
public:
    vector<int> items;
    optype flag;
    int function_num;
    int ring;
    int divisible_by;
    Monkey* first;
    Monkey* second;
    int id;
    long int inspections;
    int worry(int item) {
        int worry_level=0;
        int tmp=0;
        switch (flag) {
            case optype::addition: 
                worry_level = item+function_num;
                if (ring && worry_level>ring) worry_level-=ring;
                break;
            case optype::multiplication: 
                tmp = function_num;
                if (ring && tmp>ring) tmp -= ring;
                for (int i=0; i<tmp; i++) {
                    worry_level += item;
                    if (ring && worry_level>ring) worry_level-=ring;
                }
                break;
            case optype::square: 
                tmp = item;
                if (ring && tmp>ring) tmp-=ring;
                if (ring && tmp>ring/2) tmp=ring-tmp;
                for (int i=0; i<tmp; i++) {
                    worry_level += tmp;
                    if (ring && worry_level>ring) worry_level-=ring;
                }
                break;
        }
        if (ring==0) worry_level/=3;
        return worry_level;
    };
    bool test(int item) {
        return (item%divisible_by==0);
    };
    void throw_items_to_other_monkies(){
        for (auto item: items){
            int worry_level=worry(item);
            if (test(worry_level)){
                first->items.push_back(worry_level);
            } else {
                second->items.push_back(worry_level);
            }
        }
        items.clear();
    };
    void print();
};



void printvec(vector<int> v){
    std::cout << "[";
    for (auto i: v){
        std::cout << i << ",";
    }
    std::cout << "]";
}

void Monkey::print(){
    int tmp;
    std::cout << "Moneky " << id << ", inspections ("<< inspections << ")\n    items: ";
    printvec(items);
    std::stringstream op;
    switch (flag) {
        case optype::addition: op<<"+ "<< function_num; break;
        case optype::multiplication: op<<"* "<< function_num; break;
        case optype::square: op<<"* old "; break;
    }
    std::cout << "\n    operation: old " << op.str() << " [";
    for (auto i: items){
        std::cout << worry(i) << ",";
    }
    std::cout << "]\n    test: divisible by " << divisible_by << " [";
    for (auto i: items){
        tmp=worry(i);
        std::cout << test(worry(tmp)) << ",";
    }
    std::cout << "]\n      if true: throw to monkey " << first->id;
    std::cout << "\n      if false: throw to monkey " << second->id << std::endl;
}

bool parse_starting_items(string& line, vector<int>& items){
    int num;
    std::stringstream nums;
    string head = "  Starting items: ";
    if (line.compare(0,head.length(),head)==0){
        for (auto c: line.substr(head.length())){
            if ('0'<=c && c<='9') nums<<c;
            if (c==','){
                nums >> num; 
                items.push_back(num);
                nums.clear();
            }
        }
        nums >> num; 
        items.push_back(num);
        nums.clear();
        return true;
    }
    return false;
}


bool parse_operations(string& line, Monkey& monkey){
    std::stringstream item;
    string head = "  Operation: new = old ";
    string token;
    int num=0;
    if (line.compare(0,head.length(),head)==0){
        for (auto c: line.substr(head.length())){
            if (c==' '){
                item>>token;
                item.str("");
                item.clear();
            } else {
                item<<c;
            }
        }
        // the last token is still in item stream
        if (token=="*"){
            if (item.str() == "old") {
                monkey.flag=optype::square;
                monkey.function_num=0;
                return true;
            } else {
                monkey.flag=optype::multiplication;
                item>>monkey.function_num;
                return true;
            }

        } else if (token=="+"){
            monkey.flag=optype::addition;
            item>>monkey.function_num;
            return true;
        } else {
            return false;
        }
    }
    return false;
}

// parse a string "  Test: divisible by x" anv convert to a test function
// currently assumes we have "divisible by",
// add more when/if they show up
bool parse_test(string& line, Monkey& monkey){
    std::stringstream item;
    string head = "  Test: divisible by ";
    int num=0;
    if (line.compare(0,head.length(),head)==0){
        item << line.substr(head.length());
        item >> monkey.divisible_by;
        return true;
    }
    return false;
}

void main2(std::istream& fin, bool worrying, bool do_10000){
    string line,head;
    vector<Monkey> monkies;
    bool ret;
    vector<int> first_monkey_ind;
    vector<int> second_monkey_ind;
    int id=0;
    int ring=1;
    while (getline(fin,line)){
        // std::cout << line << std::endl;
        if (line.compare("done")==0) break; // for stdin

        if (line.compare(0,7,"Monkey ")==0) {
            Monkey &curr_monkey = monkies.emplace_back();
            curr_monkey.id=id++;
            curr_monkey.inspections=0;
            getline(fin,line);
            // std::cout <<line <<std::endl;
            ret = parse_starting_items(line,curr_monkey.items);
            if (!ret) std::invalid_argument("Monkey has no valid starting items");
            getline(fin,line);
            // std::cout <<line <<std::endl;
            ret = parse_operations(line, curr_monkey);
            if (!ret) std::invalid_argument("Monkey has no valid operation items");
            // std::cout <<parse_operations(line)(1) <<std::endl;
            getline(fin,line);
            // std::cout <<line <<std::endl;
            ret = parse_test(line, curr_monkey);
            if (!ret) std::invalid_argument("Monkey has no valid divisible by");
            ring *= curr_monkey.divisible_by;
            getline(fin,line);
            // std::cout <<line <<std::endl;
            head = "    If true: throw to monkey ";
            first_monkey_ind.push_back(std::stoi(line.substr(head.length())));
            getline(fin,line);
            // std::cout <<line <<std::endl;
            head = "    If false: throw to monkey ";
            second_monkey_ind.push_back(std::stoi(line.substr(head.length())));

        }
    }

    if (!worrying) ring=0;
    std::cout << "ring is " << ring << std::endl;
    for (int i=0; i<monkies.size(); i++) {
        monkies[i].first = &(monkies[first_monkey_ind[i]]);
        monkies[i].second = &(monkies[second_monkey_ind[i]]);
        monkies[i].ring=ring;
    }
    for (auto &monkey: monkies){
            monkey.print();
        }
    

    for (int i=0; i<20; i++){
        for (auto &monkey: monkies){
            monkey.inspections += monkey.items.size();
            monkey.throw_items_to_other_monkies();
            // std::cout << "monkey " << monkey.id << " thrown. now. " << i << ", monkey_business " << monkey.inspections << std::endl;
            // for (auto monkey: monkies){
            //     monkey.print();
            // }   
        }
        std::cout << "########### round " << i+1 << " ###########" << std::endl;
        for (auto &monkey: monkies){
            monkey.print();
        }
    }

    long unsigned int max_score=0;
    long unsigned int sec_max_score=0;
    for (auto &monkey: monkies){
        if (monkey.inspections>sec_max_score) {
            if (monkey.inspections>max_score) {
                sec_max_score = max_score;
                max_score = monkey.inspections;
            } else {
                sec_max_score = monkey.inspections;
            }
        }
    }
    std::cout << "Monkey business of the situation after 20 steps is " << max_score*sec_max_score << std::endl;
    if (!do_10000) return;
    for (int i=20; i<10000; i++){
        for (auto &monkey: monkies){
            monkey.inspections += monkey.items.size();
            monkey.throw_items_to_other_monkies();
        }
        if (i%1000==999){
            std::cout << "########### round " << i+1 << " ###########" << std::endl;
            for (auto &monkey: monkies){
                std::cout << monkey.id << ": monkey inspections " << monkey.inspections << std::endl;
            }
        }
    }
    max_score=0;
    sec_max_score=0;
    for (auto &monkey: monkies){
        if (monkey.inspections>sec_max_score) {
            if (monkey.inspections>max_score) {
                sec_max_score = max_score;
                max_score = monkey.inspections;
            } else {
                sec_max_score = monkey.inspections;
            }
        }
    }
    std::cout << "max inspections: " << max_score << ", " << sec_max_score << std::endl;
    long long unsigned int monkey_business = max_score*sec_max_score;
    std::cout << "Monkey business of the situation after 10000 steps is " << monkey_business << std::endl;

}
int main(int argc, char* argv[]) {
    ifstream inputstream;
    string buff;
    bool worryness=true, do_10000=false;
    for (int i=1; i<argc; i++) {
        buff = argv[i];
        if (buff == "--no-worry") {
            worryness = false;
            std::cout << "Dividing worry level by 3" <<std::endl;
        }
        if (buff == "--do-10-000") {
            do_10000 = true;
            std::cout << "Monkies passing around 10,000 rounds" <<std::endl;
        }
    }
    if (worryness) {std::cout << "Not dividing worry level by 3. Use {program} ... --no-worry" <<std::endl;};
    if (!do_10000) {std::cout << "Only going 20 steps. Use {program} ... --do_10-000 to do 10,000 steps." <<std::endl;};
    if (argc>1) {
        buff = argv[1];
        inputstream.open(buff);
        main2(inputstream, worryness, do_10000);
        inputstream.close();
    } else {
        main2(std::cin, worryness, do_10000);
    }
    
    return 0;
}
