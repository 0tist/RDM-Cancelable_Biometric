#include<vector>
#include<iostream>
#include<string>

using namespace std;


void print(vector<int> vec){
    for(auto i=0; i<vec.size(); i++){
        cout<<vec.at(i)<<" ";
    }
}
void allPermutationRecur(vector<int> vals, vector<int> container,
                            int last, int index)
{
    int len = vals.size();
    // One by one fix all characters at
    // the given index and recur for
    // the/ subsequent indexes
    for (auto i = 0; i < len; i++)
    {
        // Fix the ith character at index
        // and if this is not the last
        // index then recursively call
        // for higher indexes
        container[index] = vals[i] ;

        // If this is the last index then
        // print the string stored in
        // data[]
        if (index == last){
            print(container);
            cout<<endl;
        }
        else // Recur for higher indexes
            allPermutationRecur (vals, container, last, index+1);
    }
}

/* This function sorts input string,
allocate memory for data (needed
for allLexicographicRecur()) and
calls allLexicographicRecur() for
printing all permutations */
void allPermutation(vector<int> vals, int size)
{
    int len = vals.size() ;
    // Create a temp array that will
    // be used by allLexicographicRecur()
    vector<int> data(size, 0);
    // Sort the input string so that
    // we get all output strings in
    // lexicographically sorted order
    sort(vals.begin(), vals.end());
    // Now print all permutaions
    allPermutationRecur (vals, data, size-1, 0);

    // Free data to avoid memory leak
    data.clear();
}

// Needed for library function qsort()

// Driver code
int main()
{
    int vec_size, range;
    cout<<"input the size of the vector: ";
    cin>>vec_size;
    cout<<endl;
    cout<<"range of the function: ";
    cin>>range;
    cout<<endl;
    vector<int> flag;
    for(auto i=1; i<=range; i++){
        flag.push_back(i);
    }
    allPermutation(flag, vec_size);
    return 0;
}
