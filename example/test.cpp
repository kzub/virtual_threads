#include <stdio.h>
#include <virtual_threads.h>

// Example routine 1 -----------------------------------------
class MyRoutine : public VT::Routine {
  int test = 0;
  void body() {
    if (is_current_step()) {
      printf("%lu test 1\n", VT::Time::now());
      next_step();
      return;
    }
    if (is_current_step()) {
      printf("%lu test 2\n", VT::Time::now());
      next_step(200);
      return;
    }
    if (is_current_step()) {
      printf("%lu test 3\n", VT::Time::now());
      // sleep(4);
      next_step(500);
      return;
    }
    if (is_current_step()) {
      test++;
      printf("%lu test 4\n", VT::Time::now());
      // loop
      if (test < 2) {
        first_step(2000);
      }
      return;
    }
    printf("here, wtf1? %d", test);
  };
};

// Example routine 2 -----------------------------------------
class MyRoutine2 : public VT::Routine {
  int test = 0;
  void body() {
    if (is_current_step()) {
      printf("%lu EVERY SEC\n", VT::Time::now());
      if (test++ < 6) {
        repeat_step(1000);
      }
      return;
    }
    printf("here, wtf2? %d\n", test);
  };
};

// Main -----------------------------------------
int main() {
  MyRoutine ev1;
  MyRoutine2 ev2;

  auto engine = VT::Engine::getInstance();
  engine->add_routine(ev1);
  engine->add_routine(ev2);

  engine->start();
  engine->clear();

  printf("(done)\n");
}