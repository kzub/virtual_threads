#ifndef virtual_threads_h
#define virtual_threads_h

namespace VT {
  class Routine;
  // storage
  template <typename T> class Array;

  // class used to iterate trought routines array
  template <typename T> class Iterator {
    private:
      unsigned int index;
      Array<T> *array;
    public:
      Iterator(Array<T> *arr);
      T* iterate();
      void reset();
      unsigned int get_index();
  };

  // array class for storing routines
  template <typename T> class Array {
    private:
      struct Data {
        T *value;
        Data *next;
      };

      Data *base;
      Data *current;
      Data *temp;
      unsigned int _length;

    public:
      Array();
      ~Array();

      unsigned int length();
      void push(T *val);
      T* get(unsigned int index);
      Iterator<T> getIterator();
  };

  // singleton class to keep all operations
  class Engine{
    private:
      static Engine *engine;
      Array<Routine> routines;

    public:
      enum Time {
        MILLISECONDS,
        MICROSECONDS
      };

      static unsigned long get_current_time(Time tFormat);
      // add routine to stack
      static void add_routine(Routine *rt);
      // start processing all routines
      static void start();
      // clear routines list
      static void clear();
  };

  // parent class for user routine implementation
  class Routine {
    private: 
      friend class Engine;
      unsigned int current_step;
      unsigned int steps_counter;
      unsigned long next_step_time;
      bool processing;
      bool started;

      void make_one_step();
    protected:
      // must be implemented in child class
      virtual void body() = 0;

      // go to next step
      void next_step(unsigned long wait_time = 0);
      // start from the begining
      void first_step(unsigned long wait_time = 0);
      // repat current step one more time
      void repeat_step(unsigned long wait_time = 0);
      // used for if branching in child class body()
      bool is_current_step();
      bool is_complete();

      // debug methods:
      unsigned long get_current_step();
      unsigned long get_steps_counter();

    public:
      Routine():
        current_step(0),
        steps_counter(0),
        next_step_time(0),
        processing(false),
        started(false)
      {};
  };
}
#endif