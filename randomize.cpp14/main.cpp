#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

#include "randomize.hpp"

int main() {
    // (1) integral function parameter within range [1 ; 6]
    std::cout << randomize::rand(1, 6) << '\n';
    
    // (2) integral random function generation within range [1 ; 6]
    auto f = randomize::get_rand(1, 12);
    std::cout << f() << " " << f() << " " << f() << " " << f() << '\n';
    
    // (3) floating-point function parameter within range [3.14 ; 42]
    std::cout << randomize::rand(3.14, 42.) << '\n';
    
    // (4) floating-poing random function generation within  range [-16 ; 64]
    auto g = randomize::get_rand(-16., 64.);
    std::cout << g() << '\n';
    
    // (5) integral template parameter within range [-infinity(short) ; infinity(short)]
    std::cout << randomize::rand<short>() << '\n';
    
    // (6) floating-point template parameter within range [-2 ; 3]
    auto rand_double = randomize::rand<double, -2, 3>;
    std::cout << rand_double() << '\n';
    
    // (7) fill a vector with random numbers
    std::vector<float> v(randomize::rand(5, 1'000));
    std::generate(std::begin(v), std::end(v), randomize::get_rand(-100.f, 100.f));
    std::copy(std::begin(v), std::end(v), std::ostream_iterator<float>(std::cout, " "));
    
    // flush!
    std::cout << std::endl;
}
