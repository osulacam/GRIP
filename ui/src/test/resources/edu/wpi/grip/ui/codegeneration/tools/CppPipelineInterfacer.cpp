#include "edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer.h"
#include "Handle.h"
#include "AbsPipeline.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <String>
#include <dlfcn.h>
#include <vector>
typedef AbsPipeline* maker();
JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_init
  (JNIEnv *env , jobject obj, jstring libName){
      const char *lib= env->GetStringUTFChars(libName,0);
      void* libHandle = dlopen(lib, RTLD_NOW);
      maker* make = (maker*) dlsym(libHandle, "makePipeline");
      AbsPipeline *inst = make();
      inst->libHandle = libHandle;
      setHandle(env, obj, inst);
      env->ReleaseStringUTFChars(libName, lib);
  }
  
JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_process
  (JNIEnv *env , jobject obj){
      AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
      inst->Process();
  }

JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getMatNative
  (JNIEnv *env, jobject obj, jint outNum, jlong handle){
      AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
      Mat * out = (Mat *) (inst->*(inst->getOutputs()[(int) outNum]))();
      Mat * dest = reinterpret_cast<Mat *>(handle); 
      out->copyTo(*dest);
  }
  
  JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_setMatSource
  (JNIEnv *env, jobject obj, jint sourceNum, jstring fileName){
    AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
    const char *imgName= env->GetStringUTFChars(fileName,0);
    std::string name = string(imgName);
    Mat img = imread(imgName, 1);
    env->ReleaseStringUTFChars(fileName, imgName);
    (inst->*(inst->getMatSources()[(int) sourceNum]))(&img);
  }
  
  JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_setCondition
  (JNIEnv *env, jobject obj, jint boolNum, jboolean value){
      AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
      bool val = (bool) value;
      (inst->*(inst->getConditions()[(int) boolNum]))(val);
  }
  
  JNIEXPORT jboolean JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getBoolean
  (JNIEnv *env, jobject obj, jint num){
    AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
    bool * output = (bool *) (inst->*(inst->getOutputs()[(int) num]))();
    return (jboolean) *output;
  }
  
  JNIEXPORT jdouble JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getDouble
  (JNIEnv *env, jobject obj, jint num){
      AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
    double * output = (double *) (inst->*(inst->getOutputs()[(int) num]))();
    return (jdouble) *output;
  }
  
  JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_dispose
  (JNIEnv *env , jobject obj){
      AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
      void * libHandle = inst->libHandle;
      delete inst;
      dlclose(libHandle);
  }

  
  JNIEXPORT jdoubleArray JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getSizeOrPoint
  (JNIEnv *env, jobject obj, jint num, jboolean isSize){
  	int numEles = 2;
    double vals[numEles];
    AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
    if(isSize){
      Size * sz = (Size *)(inst->*(inst->getOutputs()[(int)num]))();
      vals[0] = sz->width;
      vals[1] = sz->height;
    }
    else{
      Point * pnt = (Point *)(inst->*(inst->getOutputs()[(int)num]))();
      vals[0] = pnt->x;
      vals[1] = pnt->y;
    }
    jdoubleArray retval = env->NewDoubleArray(numEles);
    env->SetDoubleArrayRegion(retval, 0, numEles, vals);
    return retval;
  }

  void KeyPointVectorToMat(vector<KeyPoint>& v_kp, Mat& mat)
{
    int count = (int)v_kp.size();
    mat.create(count, 1, CV_32FC(7));
    for(int i=0; i<count; i++)
    {
        KeyPoint kp = v_kp[i];
        mat.at< Vec<float, 7> >(i, 0) = Vec<float, 7>(kp.pt.x, kp.pt.y, kp.size, kp.angle, kp.response, (float)kp.octave, (float)kp.class_id);
    }
}

  JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getBlobs
  (JNIEnv *env, jobject obj, jint num, jlong outAdr){
    AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
    vector<KeyPoint> * output = (vector<KeyPoint> *) (inst->*(inst->getOutputs()[(int) num]))();
    Mat* out = (Mat*) outAdr;
    KeyPointVectorToMat(*output, *out);    
  }

JNIEXPORT jint JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getNumContours
  (JNIEnv *env, jobject obj, jint num){
  AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
  vector<vector<Point> > * output = (vector<vector<Point> > *) (inst->*(inst->getOutputs()[(int) num]))();
  return (jint) output->size();
}

JNIEXPORT void JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getContours
  (JNIEnv *env, jobject obj, jint num, jlongArray addresses){
  AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
  vector<vector<Point> > * output = (vector<vector<Point> > *) (inst->*(inst->getOutputs()[(int) num]))();
  jsize len = env->GetArrayLength(addresses);
  jlong *addrs = env->GetLongArrayElements(addresses, 0);
  for(int idx = 0; idx < len; idx++){
    Mat temp = Mat((*output)[idx], true);
    temp.copyTo(*(Mat *) addrs[idx]);
  }
  env->ReleaseLongArrayElements(addresses, addrs, 0);
}
/*extern "C" vector<Vec6d> getLines(int num, AbsPipeline* obj){
	vector<Line> lines = (obj->*(obj->getOutputs()[num]))();
	vector<Vec6d> out;
	for(int idx = 0; idx<lines.size(); idx++){
		Line l = lines[idx];
		out.push_back(Vec6d(l.x1, l.y1, l.x2, l.y2, l.length(), l.angle()));
	}
	return out;
}*/


typedef vector<Vec6d> LineFun(int, AbsPipeline*);
JNIEXPORT jobjectArray JNICALL Java_edu_wpi_grip_ui_codegeneration_tools_CppPipelineInterfacer_getLines
  (JNIEnv *env, jobject obj, jint num){
  AbsPipeline *inst = getHandle<AbsPipeline>(env, obj);
  LineFun* getLines =(LineFun *) dlsym(inst->libHandle, "getLines");
  vector<Vec6d> lines = getLines((int)num, inst);
  jclass dblArray = env->FindClass("[D");
  jint numLines = lines.size();
  jint numEles = 6;
  jobjectArray linesArray = env->NewObjectArray(numLines, dblArray, NULL);
  for(jint idx = 0; idx< numLines; idx++){
  	jdoubleArray data = env->NewDoubleArray(numEles);
  	Vec6d line = lines[idx];
  	jdouble dblData[numEles];
  	for(int ele = 0; ele<numEles; ele++){
  		dblData[ele] = line[ele];
  	}
  	env->SetDoubleArrayRegion(data, 0, numEles, dblData);
  	env->SetObjectArrayElement(linesArray, idx, data);
  }
  return linesArray;
}