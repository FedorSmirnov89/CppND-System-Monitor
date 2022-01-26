// The file with the main method calling all the tests

#include "testFormat.cpp"
#include "testParser.cpp"


int main(){
    testFormatSuite();
    testParser();
    

    cout << "Tests executed. All assertions hold.\n";
}