#include <iostream>
#include "Core.h"
#include "Functions.h"
#include "Tasks.h"

using std::cout;
using std::endl;

void task::addition_8b_16b(bool enable_simd) {
  // Initialization
  size_t x_size = 512, y_size = 512;
  auto* pirate_img = __malloc<uint8_t>(x_size * y_size);
  auto* glow_img = __malloc<uint8_t>(x_size * y_size);
  auto* add8_c_img = __malloc<uint8_t>(x_size * y_size);
  auto* add16_c_img = __malloc<uint16_t>(x_size * y_size);
  auto* add8_simd_img = enable_simd ? __malloc<uint8_t>(x_size * y_size) : nullptr;
  auto* add16_simd_img = enable_simd ? __malloc<uint16_t>(x_size * y_size) : nullptr;
  cout << _$m << "<8-bit & 16-bit Addition>" << _$x << endl;

  // Load image(s)
  cout << "Opening image for addition... ";
  __bulk_load<uint8_t>(fileples {
    $("images/pirate_512_8b.raw", pirate_img, x_size, y_size),
    $("images/glow_512_8b.raw", glow_img, x_size, y_size)
  });
  cout << "OK" << endl;

  // Execute function(s)
  ExecResult* r = nullptr;
  veriples verify_list;
  cout << "Testing 8-bit addition... ";
  r = __exec<uint8_t, uint8_t>(c::add_8b, simd::add_8b, enable_simd, pirate_img, glow_img,
                               add8_c_img, add8_simd_img, x_size, y_size);
  if ((r->error1 == nullptr) && (r->error2 == nullptr))
    verify_list.push_back($("8-bit addition", add8_c_img, add8_simd_img, x_size, y_size));
  else
    cout << "[not comparable] ";
  delete r->print();
  cout << "Testing 16-bit addition... ";
  r = __exec<uint8_t, uint16_t>(c::add_16b, simd::add_16b, enable_simd, pirate_img, glow_img,
                                add16_c_img, add16_simd_img, x_size, y_size);
  if ((r->error1 == nullptr) && (r->error2 == nullptr))
    verify_list.push_back($("16-bit addition", add16_c_img, add16_simd_img, x_size, y_size));
  else
    cout << "[not comparable] ";
  delete r->print();

  // Verify results using comparison
  if (enable_simd) {
    cout << "Verifying results... ";
    if (__bulk_diff<uint8_t>(verify_list)) {
      cout << "OK" << endl;
    }
  }

  // Save image(s)
  cout << "Saving results... ";
  __file<uint8_t>("output/c_add8.raw", add8_c_img, x_size, y_size, "wb");
  __file<uint16_t>("output/c_add16.raw", add16_c_img, x_size, y_size, "wb");
  if (enable_simd) {
    __file<uint8_t>("output/simd_add8.raw", add8_simd_img, x_size, y_size, "wb");
    __file<uint16_t>("output/simd_add16.raw", add16_simd_img, x_size, y_size, "wb");
  }
  cout << "OK" << endl;
};
