# MMCC
<br>

### overlook
A mini make c compiler including

* preprocessing
* scanning (lexical analysis)
* parsing (syntax analysis)
* symantic analysis & icgen (intermediate code generation)

to compile a programe wrote by a subset of c language

### code structures
* algutil.h            basic algrithms, including string processing, obtaining first, follow set
* base.h               declearation for all data struct, hash table
* ioutil.h             basic input or output, xml read and write
* mmcc.cpp             main project
* pp.cpp               preprocessing, fitter comment
* scanning.cpp         output .token.xml
* parsing.cpp          output .tree.xml 
* semantic.cpp         output .ic.xml

### details
* The compiler's architecture detailed methods are in 
[mmcc_design](https://github.com/YuriSizuku/MMCC/blob/master/doc/mmcc_design.txt "mmcc_design")
* The intermediate results for debug are in 
[debug_view](https://github.com/YuriSizuku/MMCC/blob/master/samples/debugview_v0.2.9.txt "debug_view")
![](https://github.com/YuriSizuku/MMCC/blob/master/samples/mmcc_p1.png)
![](https://github.com/YuriSizuku/MMCC/blob/master/samples/mmcc_p2.png)
