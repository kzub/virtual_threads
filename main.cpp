#define MAIN_FUNCTION_DEFINED
#include "virtual_threads.h"
#include <stdio.h>

class MyRoutine: public VT::Routine {
  int test;
  public: MyRoutine(){
    test = 0;
  }
  void body(){
    if(is_current_step()){
      printf("test 1\n");
      next_step();
      return;
    }
    if(is_current_step()){
      printf("test 2\n");
      next_step(200);
      return;
    }
    if(is_current_step()){
      printf("test 3\n");
            // sleep(4);
      next_step(500);
      return;
    }
    if(is_current_step()){
      test++;
      printf("test 4\n");
            // loop
      if(test < 2){
        first_step(2000);
      }
      return;
    }
    printf("here, wtf1? %d", test);
  };
};

class MyRoutine2: public VT::Routine {
  int test;
  public: MyRoutine2(){
    test = 0;
  }
  void body(){
    if(is_current_step()){
      printf("EVERY SEC\n");
      if(test++ < 6){
        repeat_step(1000);
      }
      return;
    }
    printf("here, wtf2? %d\n", test);
  };
};

int main(){
  MyRoutine  ev1;
  MyRoutine2 ev2;

  VT::Engine::add_routine(&ev1);
  VT::Engine::add_routine(&ev2);

  VT::Engine::start();
  VT::Engine::clear();

  printf("(done)\n");

}