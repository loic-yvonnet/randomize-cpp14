<h1>Simple random numbers generation in C++14</h1>

Uses the random and chrono C++11 libraries to generate random values. This is simply syntatic sugar on top of the STL facilities.
The generated random numbers are different for each execution thanks to a chrono-based seed.

Example:

```cpp
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
```

Possible output:
```
1
11 11 7 10
21.2455
27.2841
-195
2.35654
-77.7947 -6.70831 -55.4828 -1.63568 -16.6268 -18.548 68.2206 39.1133 80.6457 56.2356 62.6092 8.67204 73.2448 -52.9134 8.52956 -41.279 9.10265 -38.5435 -41.1627 -57.4094 46.8579 90.3121 17.4146 -57.4075 -92.4222 40.1183 -53.3832 82.6493 -23.5288 -68.4938 -32.124 22.2141 44.7773 2.04337 83.7952 72.994 -83.5966 89.577 92.6692 -92.4725 -67.0379 -44.7442 88.8432 -46.1004 
```
