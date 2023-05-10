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

int ring=1;

class Monkey {
public:
    vector<int> items;
    std::function<int(int)> operation;
    std::function<bool(int)> test;
    Monkey* first;
    Monkey* second;
    int id;
    int business;
    void throw_items_to_other_monkies(){
        for (auto item: items){
            int worry_level = operation(item);
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


void main2(std::istream& fin, bool worrying);

int main(int argc, char* argv[]) {
    ifstream inputstream;
    string buff;
    bool worryness=true;
    for (int i=1; i<argc; i++) {
        buff = argv[i];
        if (buff == "--no-worry") {
            worryness = false;
            std::cout << "Dividing worry level by 3" <<std::endl;
        }
    }
    if (worryness) {std::cout << "Not dividing worry level by 3. Use {program} ... --no-worry" <<std::endl;};
    if (argc>1) {
        buff = argv[1];
        inputstream.open(buff);
        main2(inputstream, worryness);
        inputstream.close();
    } else {
        main2(std::cin, worryness);
    }
    
    return 0;
}

void printvec(vector<int> v){
    std::cout << "[";
    for (auto i: v){
        std::cout << i << ",";
    }
    std::cout << "]";
}

void Monkey::print(){
    int tmp;
    std::cout << "Moneky " << id << ", business ("<< business << ")\n    items: ";
    printvec(items);
    std::cout << "\n    operation: [";
    for (auto i: items){
        std::cout << operation(i) << ",";
    }
    std::cout << "]\n    test: [";
    for (auto i: items){
        tmp=operation(i);
        std::cout << test(tmp) << ",";
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


std::function<int (int)> parse_operations(string& line){
    std::stringstream item;
    vector<string> tokens;
    std::function<int (int)> func;
    string head = "  Operation: new = ";
    string token;
    int num=0;
    if (line.compare(0,head.length(),head)==0){
        for (auto c: line.substr(head.length())){
            if (c==' '){
                item>>token;
                tokens.push_back(token);
                item.str("");
                item.clear();
            } else {
                item<<c;
            }
        }
        // the last token is still in item stream
        if (item.str() == "old") {
            func = [](int old) {return old;};
        } else {
            item>>num;
            func = [num](int old) {return num;};
        }
        // run over the rest if tokens two at a time: ... []fourth  (fifth, []second(third, []{first}) )
        while (!tokens.empty()){
            token = tokens.back();
            tokens.pop_back();
            if (tokens.empty()) {
                // only relevant if there is a (-) ahead, otherwise quit
                if (token=="-"){
                    func = [func](int old) {return -func(old);};
                }
                break;
            }
            // we know there is another token
            if (token=="*") {
                if (tokens.back()=="old"){
                    func = [func](int old) {int tmp=func(old), acc=0, loc_ring=ring, tmp2=old; for (int i=0; i<tmp2; i++) {acc+=(tmp); if (acc>ring) acc-=loc_ring;} return acc;}; // could overflow
                } else {
                    num = stoi(tokens.back());
                    func = [func,num](int old) {int tmp=func(old), acc=0, loc_ring=ring, tmp2=num; for (int i=0; i<tmp2; i++) {acc+=(tmp); if (acc>ring) acc-=loc_ring;} return acc;}; // could overflow

                }
                tokens.pop_back();
            } else if (token=="+") {
                if (tokens.back()=="old"){
                    func = [func](int old) {int n = old+func(old); if (n>ring) n-=ring; return n;};
                } else {
                    num = stoi(tokens.back());
                    func = [func,num](int old) {int n = num+func(old); if (n>ring) n-=ring; return n;};
                }
                tokens.pop_back();
            }
        }
        return func;
    }
    return [](int old) {return old;};
}

// parse a string "  Test: divisible by x" anv convert to a test function
// currently assumes we have "divisible by",
// add more when/if they show up
std::function<bool (int)> parse_test(string& line, int &ring){
    std::stringstream item;
    vector<string> tokens;
    std::function<int (int)> func;
    std::function<bool (int)> test_func;
    string head = "  Test: ";
    string token;
    int num=0;
    if (line.compare(0,head.length(),head)==0){
        for (auto c: line.substr(head.length())){
            if (c==' '){
                item>>token;
                tokens.push_back(token);
                item.str("");
                item.clear();
            } else {
                item<<c;
            }
        }
        // the last token is still in item stream
        if (item.str() == "old") {
            func = [](int old) {return old;};
        } else {
            item>>num;
            func = [num](int old) {return num;};
            ring=num;
        }
        // run over the rest if tokens two at a time: ... []fourth  (fifth, []second(third, []{first}) )
        while (!tokens.empty()){
            token = tokens.back();
            tokens.pop_back();
            if (tokens.empty()) {
                // broken?
                break;
            }
            // we know there is another token
            if (token=="by") {
                if (tokens.back()=="divisible"){
                    return [func](int old) {return (old%func(old))==0;};
                }
                tokens.pop_back();
            } // greater than?
        }
    }
    return [](int old) {return true;};
}

void main2(std::istream& fin, bool worrying){
    string line,head;
    vector<Monkey> monkies;
    bool ret;
    vector<int> first_monkey_ind;
    vector<int> second_monkey_ind;
    int id;
    int num=1, loc_ring=1;
    while (getline(fin,line)){
        // std::cout << line << std::endl;
        if (line.compare("done")==0) break; // for stdin

        if (line.compare(0,7,"Monkey ")==0) {
            Monkey &curr_monkey = monkies.emplace_back();
            curr_monkey.id=id++;
            curr_monkey.business=0;
            getline(fin,line);
            // std::cout <<line <<std::endl;
            ret = parse_starting_items(line,curr_monkey.items);
            if (!ret) std::invalid_argument("Monkey has no starting items");
            getline(fin,line);
            // std::cout <<line <<std::endl;
            auto lam = parse_operations(line);
            if (!worrying) {
                curr_monkey.operation = [lam](int old) {return lam(old)/3;};
            } else {
                curr_monkey.operation = lam;
            }
            // std::cout <<parse_operations(line)(1) <<std::endl;
            getline(fin,line);
            // std::cout <<line <<std::endl;
            curr_monkey.test = parse_test(line, num);
            std::cout << "got num: " <<num <<std::endl;
            loc_ring *= num;
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
    ring = loc_ring;
    if (!worrying) ring=INT_MAX;
    for (int i=0; i<monkies.size(); i++) {
        monkies[i].first = &(monkies[first_monkey_ind[i]]);
        monkies[i].second = &(monkies[second_monkey_ind[i]]);
    
    }
    for (auto &monkey: monkies){
            monkey.print();
        }
    

    for (int i=0; i<20; i++){
        for (auto &monkey: monkies){
            monkey.business += monkey.items.size();
            monkey.throw_items_to_other_monkies();
            // std::cout << "monkey " << monkey.id << " thrown. now. " << i << ", monkey_business " << monkey.business << std::endl;
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
        if (monkey.business>sec_max_score) {
            if (monkey.business>max_score) {
                sec_max_score = max_score;
                max_score = monkey.business;
            } else {
                sec_max_score = monkey.business;
            }
        }
    }
    std::cout << "Monkey business of the situation after 20 steps is " << max_score*sec_max_score << std::endl;

    for (int i=20; i<10000; i++){
        for (auto &monkey: monkies){
            monkey.business += monkey.items.size();
            monkey.throw_items_to_other_monkies();
        }
        if (i%1000==999){
            std::cout << "########### round " << i+1 << " ###########" << std::endl;
            for (auto &monkey: monkies){
                monkey.print();
            }
        }
    }
    max_score=0;
    sec_max_score=0;
    for (auto &monkey: monkies){
        if (monkey.business>sec_max_score) {
            if (monkey.business>max_score) {
                sec_max_score = max_score;
                max_score = monkey.business;
            } else {
                sec_max_score = monkey.business;
            }
        }
    }
    std::cout << "Monkey business of the situation after 10000 steps is " << max_score*sec_max_score << std::endl;

}
