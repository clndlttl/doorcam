#include <iostream>

template <class T>
class Smoother {
  float _alpha;
  float _mean;
  float _power;
 
 public:
  Smoother(T init_value, float alpha) : _alpha(alpha) {
    _mean = static_cast<float>(init_value);
    _power = 0.f;
  }

  void update(T obs) {
    float fobs = static_cast<float>(obs);
    _mean = _alpha * _mean +
            (1.f - _alpha) * fobs;
    _power = _alpha * _power +
                (1.f - _alpha) * ( fobs * fobs );
  }

  T mean() {
    return static_cast<T>(_mean);
  }

  float var() {
    return (_power - ( _mean * _mean ));
  }

  void show() {
    std::cout << "mean: " << mean() << std::endl;
    std::cout << "vari: " << var() << std::endl;
    std::cout << std::endl;
  }
};
