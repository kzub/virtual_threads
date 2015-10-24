#ifndef Arduino_h
  #include <stdio.h>
  #include <sys/time.h>
  unsigned long millis() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    unsigned long ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
      // printf("%ld\n", ms);
    return ms;
  }
  #include <unistd.h>
  #define delay(x) usleep((useconds_t)x*1000)
  #define micros(x) 0 // not implemented yet
#endif
#include "virtual_threads.h"

namespace VT {
  // ********* Array() *********
  // array class for storing routines
  template <typename T> Array<T>::Array() {
    base = new Data;
    base->next = NULL;
    current = base;
    _length = 0;
  }

  template <typename T> Array<T>::~Array() {
    current = base;
    do{
      temp = current->next;
      delete current;
      current = temp;
    }while(current);
  };

  template <typename T> void Array<T>::push(T *val){
    if(val == NULL){
      // bad pointer
      return;
    }

    current->value = val;
    current->next = new Data;
    current->next->next = NULL;
    current = current->next;
    _length++;
  };

  template <typename T> unsigned int Array<T>::length(){
    return _length;
  }

  template <typename T> T* Array<T>::get(unsigned int index){
    if(index >= _length){
      return NULL;
    }

    Data *element = base;

    for(int i = 0; i <= index; i++){
      if(i == index){
        return element->value;
      }
      element = element->next;
    }
    // throw "impossible variant";
    return NULL;
  };

  template <typename T> Iterator<T> Array<T>::getIterator(){
    return Iterator<T>(this);
  };

  // ********* Iterator() *********
  // class used to iterate trought routines array
  template <typename T> Iterator<T>::Iterator(Array<T> *arr){
    index = 0;
    array = arr;
  };

  template <typename T> T* Iterator<T>::iterate(){
    // last element; iteration is overs
    if(index >= array->length()){
      return NULL;
    }
    return array->get(index++);
  }

  template <typename T> void Iterator<T>::reset(){
    index = 0;
  }

  template <typename T> unsigned int Iterator<T>::get_index(){
    return index - 1;
  }

  // ********* Routine() *********
  void Routine::make_one_step(){
    processing = false;
    started = true;
    steps_counter = 0;
    next_step_time = 0;

    // call user defined program in child body
    body();
  };

  void Routine::next_step(unsigned long wait_time){
    current_step++;
    processing = true;
    if(wait_time){
      next_step_time = Engine::get_current_time(Engine::MILLISECONDS) + wait_time;
    }
  };

  // start from the begining
  void Routine::first_step(unsigned long wait_time){
    current_step = 0;
    processing = true;
    if(wait_time){
      next_step_time = Engine::get_current_time(Engine::MILLISECONDS) + wait_time;
    }
  }

  // repat current step one more time
  void Routine::repeat_step(unsigned long wait_time){
    processing = true;
    if(wait_time){
      next_step_time = Engine::get_current_time(Engine::MILLISECONDS) + wait_time;
    }
  }

  // used for if branching in child class body()
  bool Routine::is_current_step(){
    return steps_counter++ == current_step;
  };

  bool Routine::is_complete(){
    return started & !processing;
  }

  // debug methods:
  unsigned long Routine::get_current_step(){
    return current_step;
  }

  unsigned long Routine::get_steps_counter(){
    return steps_counter;
  }

  // singleton class to keep all operations
  // ********* Engine() *********
  // singleton initialization
  Engine *Engine::engine = NULL;

  unsigned long Engine::get_current_time(Time tFormat){
    if(tFormat == MILLISECONDS){
      return millis();
    }
    if(tFormat == MICROSECONDS){
      return micros();
    }

    // throw "not implemented yet";
    return NULL;
  };

  void Engine::add_routine(Routine *rt){
    if(!engine){
      engine = new Engine();
    }
    engine->routines.push(rt);
  };

  void Engine::start(){
    if(!engine){
  		// no routines were added
      return;
    }

    Routine *rt;
    Iterator<Routine> routines_iterator = engine->routines.getIterator();

    bool in_progress = false;
    bool nearest_time_setted_up = false;
    unsigned long nearest_step_time = 0;
    unsigned long current_time;
    unsigned long wait_time;

    while(true){
      current_time = get_current_time(MILLISECONDS);
      rt = routines_iterator.iterate();

      // if all routines did one step
      if(!rt){
    		// and no routines in progress so stop the engine.
        if(!in_progress){
          break;
        }

        // calculate if there are any time to have a sleep?
        if(nearest_step_time > current_time){
          wait_time = nearest_step_time - current_time;
          if(wait_time > 0){
            printf("wait %lu\n", wait_time);
            delay(wait_time);
          }
        }

        // begin new turn and continue to process all routenes one more time
        routines_iterator.reset();
        nearest_time_setted_up = false;
        in_progress = false;
        continue;
      }

      // skip routines that finish thier job
      if(rt->is_complete()){
        continue;
      }

      in_progress = true;
      // execute routine body() if time frame is appropriate
      if(current_time >= rt->next_step_time){
        // current routine tell us that it still in process. So we must come back to it later
        rt->make_one_step();
      }

      if(!nearest_time_setted_up){
        nearest_time_setted_up = true;
        nearest_step_time = rt->next_step_time;
      }

      // find minimal sleep time for all routines
      if(rt->next_step_time < nearest_step_time){
        nearest_step_time = rt->next_step_time;
      }
    }
  };

  void Engine::clear(){
    delete engine;
    engine = NULL;
  };
} // VIRTUALTHREADS namespase

