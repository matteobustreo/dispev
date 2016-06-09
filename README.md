# dispev
**dispev** is a C++ implementation of the matching algorithm for stereoscopic DISParity EValuation described in [A two-stage correlation method for stereoscopic depth estimation](html://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=5692569)
 by Nils Einecke, and Julian Eggert (DICTA, page 227-234. IEEE Computer Society, 2010).

![alt tag](http://imgur.com/ZLMHmNV)
 
Algorithm implementation uses OpenCV 2.4.10 and it is preconfigured for being compiled with Visual Studio 2010.
All the required dependancy files are included in the zip file (version x64, vc10).

"bin/" folder contains a precompiled version of the code and a .bat file for running the algorithm in some test images.
A Matlab file is provided for analysing the results. 

*Note:* dispev implementation is not optimized in speeded of execution!

## Usage
Usage Example: 

```Dispev.exe C:\\leftPic.jpg C:\\rightPic.jpg  maxDisp minDisp boxFilterSizeX boxFilterSizeY sumFilterSizeX sumFilterSizeX```

where ```maxDisp``` and ```minDisp``` represent the disparity interval the algorithm will search for correspondence. If not inserted ```maxDisp``` and ```minDisp``` are set to 60 and 1.
	
```(boxFilterSizeX, boxFilterSizeY)``` represents the correlation filter size. If not inserted ```(boxFilterSizeX, boxFilterSizeY)``` is set to (3,3).
	
```(sumFilterSizeX, sumFilterSizeY)``` represents the summation filter size. If not inserted ```(sumFilterSizeX, sumFilterSizeY)``` is set to (5,9).

```Dispev.bat``` shows an usage example.

The Matlab file ```visualizeResults.m``` displays the results and generates some statistics on them (exe and images paths have to be modified conveniently before running the file!!). 
	
## See also
[www.matteobustreo.com](http://www.matteobustreo.com)