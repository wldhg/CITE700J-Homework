#include <iostream>
#include "Functions.h"
#include "Tasks.h"
#include "Util.h"

using std::cout;
using std::endl;

void task::sobel_filter(bool enable_simd) {
	// Initialization
	size_t x_size = 512, y_size = 512;
	auto* lena_img = __malloc<uint8_t>(x_size * y_size);
	auto* zp_c_img = __malloc<uint8_t>(x_size * y_size);
	auto* be_c_img = __malloc<uint8_t>(x_size * y_size);
	auto* zp_simd_img = enable_simd ? __malloc<uint8_t>(x_size * y_size) : nullptr;
	auto* be_simd_img = enable_simd ? __malloc<uint8_t>(x_size * y_size) : nullptr;
	cout << _$m << "<Sobel Filter>" << _$x << endl;

	// Load image(s)
	cout << "Opening image for filtering... ";
	__file<uint8_t>("images/lena_512.raw", lena_img, x_size, y_size, "r");
	cout << "OK" << endl;

	// Execute function(s)
	ExecResult* r = nullptr;
	bool do_verify = true;
	cout << "Testing zero-pad Sobel filter... ";
	r = __exec<uint8_t>(c::sobel_zp, simd::sobel_zp, enable_simd, lena_img, zp_c_img, zp_simd_img,
											x_size, y_size);
	do_verify &= (r->c_error != nullptr && r->simd_error != nullptr);
	delete r->print();
	cout << "Testing boundary extension Sobel filter... ";
	r = __exec<uint8_t>(c::sobel_be, simd::sobel_be, enable_simd, lena_img, be_c_img, be_simd_img,
											x_size, y_size);
	do_verify &= (r->c_error != nullptr && r->simd_error != nullptr);
	delete r->print();

	// Verify results using comparison
	if (enable_simd) {
		if (do_verify) {
			cout << "Verifying results... ";
			__bulk_diff<uint8_t>(veriples {
				$("zero-pad Sobel filter", zp_c_img, zp_simd_img, x_size, y_size),
				$("boundary extension Sobel filter", be_c_img, be_simd_img, x_size, y_size)
			});
		} else {
			cout << "Verification condition was not satisfied. Skipping verification." << endl;
		}
	}

	// Save image(s)
	cout << "Saving results... ";
	__bulk_save<uint8_t>(fileples {
		$("output/c_sobel_zp.raw", zp_c_img, x_size, y_size),
		$("output/c_sobel_be.raw", be_c_img, x_size, y_size)
	});
	if (enable_simd) {
		__bulk_save<uint8_t>(fileples {
			$("output/simd_sobel_zp.raw", zp_simd_img, x_size, y_size),
			$("output/simd_sobel_be.raw", be_simd_img, x_size, y_size)
		});
	}
	cout << "OK" << endl;
};
