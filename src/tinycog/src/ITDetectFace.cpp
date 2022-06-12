/*
  Project: OpenCogER
  File: ITDetectFace.cpp
  Author: Mandeep Singh Bhatia
  License: AGPL
  Date: May 2017
*/
#include "ITDetectFace.hpp"


ITDetectFace::ITDetectFace(string t_name):ImageTransform(t_name)
{
    //ctor
    if( !it_cascade.load( cascade_file ) ){ cout<<"--(!)Error loading face cascade xml\n"; };
}

vector<Rect> ITDetectFace::Transform(Mat in)
{
    vector<Rect> out;
    it_cascade.detectMultiScale( in, out, 1.1, 4, 0|CV_HAAR_SCALE_IMAGE, Size(55, 55) );
    return out;
}
