# Decision-Tree
- Implement a decision tree algorithm

### Dataset
- Source: https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/heart_scale
- The data file is assumed be of the famous LIBSVM sparse format. 

### Input
- The program reads the input examples that contain numerical values, and print out a piece of C code representing the decision tree.
- The program takes the data file as the first argument, and ε as the second argument. For instance, ./tree heart scale 0.1 learns a decision tree from the data file heart scale with ε = 0.1.

### Output
- Output the tree as a function in C/C++ language. The function follows the interface:
   int tree_predict(double *attr);
- The only argument is a double array which contains the factors of one example in the same format as input. 
- The function returns the label prediction of the example (1 or -1 for heart scale, for instance). 
