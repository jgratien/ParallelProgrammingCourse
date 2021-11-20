# Documentation for all functions and classes in folder myMath

## Class vec2

### Attributes
```cpp
float x;
float y;
```

### Initialization
```cpp
public:
    Vec2(); // return vec2(0, 0)
    Vec2(float, float);
    Vec2(float);
    ~Vec2() {};
```

### Methods

```cpp
public:
  Vec2 operator+(const Vec2& other); // add vectors
  Vec2& operator+=(const Vec2& other); // add vectors

  Vec2 operator-(const Vec2& other); // substract vectors
  Vec2& operator-=(const Vec2& other); // substract vectors

  Vec2 operator*(const Vec2& other); // multiply vectors term by term
  Vec2 operator*(float other); // scale the vector

  Vec2 operator/(const Vec2& other); // divide vectors term by term
  Vec2 operator/(float other); // scale the vector

  Vec2& operator*=(const Vec2& other); // multiply vectors term by term
  Vec2& operator*=(float other); // scale the vector

  Vec2& operator/=(const Vec2& other); // divide vectors term by term
  Vec2& operator/=(float other); // scale the vector

  Vec2& operator=(const Vec2& other); // assign vector
  bool operator==(const Vec2& other); // compare current vector with vector

  float dot(const Vec2& other); // return the scalar product
  float norm(); // return the norm of the vector
  Vec2 normalize(); // return the current vector normalized
  float angle(); // return the angle between the current vector and `Vec2(1, 0)`
  float angle(Vec2& other); // return the angle between the current vector and an other vector
  Vec2 rotate(float angle); // rotate the vector anti-clockwise
  std::string string(); // return a string useful when you need to display the vector

std::ostream& operator<<(std::ostream& os, Vec2& obj);
```

## Functions

### distance
```cpp
double distance(vec2 a, vec2 b) // return the distance between `a` and `b`
```

### barycenter
```cpp
vec2 barycenter(std::vector<vec2> vecs) // return a `vec2` which has the coordinates of the barycenter of all `vec2`
```

### radians and degrees
```cpp
float radians(float angle); // convert the degree angle to radian angle
float degrees(float angle); // convert the radian angle to degree angle
```

### isAntiClockwise
```cpp
bool isAntiClockwise(Vec2 a, Vec2 b); // return `true` if `a` and `b` are anti clockwise else `false`
```
