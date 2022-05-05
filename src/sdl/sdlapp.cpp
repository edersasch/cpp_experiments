#include "sdlapp.h"

#include "SDL_image.h"

#include <string_view>
#include <initializer_list>

static constexpr std::string_view muehle_board_svg = R"""(<?xml version="1.0" encoding="utf-8"?>
        <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" height="350" width="350">
            <rect width="350" height="350" style="fill: white"/>
            <rect x="20" y="20" width="310" height="310" style="fill: black"/>
            <rect x="30" y="30" width="290" height="290" style="fill: white"/>
            <rect x="70" y="70" width="210" height="210" style="fill: black"/>
            <rect x="80" y="80" width="190" height="190" style="fill: white"/>
            <rect x="120" y="120" width="110" height="110" style="fill: black"/>
            <rect x="170" y="25" width="10" height="300" style="fill: black"/>
            <rect x="25" y="170" width="300" height="10" style="fill: black"/>
            <rect x="130" y="130" width="90" height="90" style="fill: white"/>
        </svg>)""";

/*static constexpr std::string_view back_svg = R"""(<?xml version="1.0" encoding="utf-8"?>
        <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" height="22pt" width="22pt" viewBox="0 0 128 128">
            <path transform="rotate(135, 63.8945, 64.1752)" d="m109.07576,109.35336c-1.43248,1.43361 -3.41136,2.32182 -5.59717,2.32182l-79.16816,0c-4.36519,0 -7.91592,-3.5444 -7.91592,-7.91666c0,-4.36337 3.54408,-7.91667 7.91592,-7.91667l71.25075,0l0,-71.25075c0,-4.3652 3.54442,-7.91592 7.91667,-7.91592c4.36336,0 7.91667,3.54408 7.91667,7.91592l0,79.16815c0,2.1825 -0.88602,4.16136 -2.3185,5.59467l-0.00027,-0.00056z"/>
        </svg>)""";

static constexpr std::string_view cancel_svg = R"""(<?xml version="1.0" encoding="utf-8"?>
        <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" height="22pt" width="22pt" viewBox="0 0 128 128">
            <g fill-rule="evenodd">
                <path d="M64,76.3137085 L47.0294734,93.2842351 C43.9038742,96.4098343 38.8399231,96.4084656 35.7157288,93.2842712 C32.5978915,90.166434 32.5915506,85.0947409 35.7157649,81.9705266 L52.6862915,65 L35.7157649,48.0294734 C32.5901657,44.9038742 32.5915344,39.8399231 35.7157288,36.7157288 C38.833566,33.5978915 43.9052591,33.5915506 47.0294734,36.7157649 L64,53.6862915 L80.9705266,36.7157649 C84.0961258,33.5901657 89.1600769,33.5915344 92.2842712,36.7157288 C95.4021085,39.833566 95.4084494,44.9052591 92.2842351,48.0294734 L75.3137085,65 L92.2842351,81.9705266 C95.4098343,85.0961258 95.4084656,90.1600769 92.2842712,93.2842712 C89.166434,96.4021085 84.0947409,96.4084494 80.9705266,93.2842351 L64,76.3137085 Z M64,129 C99.346224,129 128,100.346224 128,65 C128,29.653776 99.346224,1 64,1 C28.653776,1 1.13686838e-13,29.653776 1.13686838e-13,65 C1.13686838e-13,100.346224 28.653776,129 64,129 Z M64,113 C90.509668,113 112,91.509668 112,65 C112,38.490332 90.509668,17 64,17 C37.490332,17 16,38.490332 16,65 C16,91.509668 37.490332,113 64,113 Z"></path>
             </g>
        </svg>)""";

static constexpr std::string_view delete_svg = R"""(<?xml version="1.0" encoding="utf-8"?>
        <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" height="22pt" width="22pt" viewBox="0 0 128 128">
            <g fill-rule="evenodd" transform="translate(12.000000, 0.000000)">
                <rect x="0" y="11" width="105" height="16" rx="8"></rect>
                <rect x="28" y="0" width="48" height="16" rx="8"></rect>
                <rect x="8" y="16" width="16" height="112" rx="8"></rect>
                <rect x="8" y="112" width="88" height="16" rx="8"></rect>
                <rect x="80" y="16" width="16" height="112" rx="8"></rect>
                <rect x="56" y="16" width="16" height="112" rx="8"></rect>
                <rect x="32" y="16" width="16" height="112" rx="8"></rect>
            </g>
        </svg>)""";

static constexpr std::string_view depth_svg = R"""(<?xml version="1.0" encoding="utf-8"?>
        <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" height="22pt" width="22pt" viewBox="0 0 128 128">
            <g fill-rule="evenodd">
                <path d="M128,-97 L112.992786,-97 C112.452362,-97 112,-96.5522847 112,-96 C112,-95.4438648 112.444486,-95 112.992786,-95 L128,-95 L128,-97 Z M128,-78.6794919 L111.216185,-88.3696322 C110.748163,-88.6398444 110.132549,-88.4782926 109.856406,-88 C109.578339,-87.5183728 109.741342,-86.9117318 110.216185,-86.6375814 L128,-76.3700908 L128,-78.6794919 Z M78.6794919,-128 L88.3696322,-111.216185 C88.6437826,-110.741342 88.4816272,-110.134474 88,-109.856406 C87.5217074,-109.580264 86.9077936,-109.748163 86.6375814,-110.216185 L76.3700908,-128 L78.6794919,-128 Z M97,-128 L97,-112.992786 C97,-112.444486 96.5561352,-112 96,-112 C95.4477153,-112 95,-112.452362 95,-112.992786 L95,-128 L97,-128 Z M115.629909,-128 L105.362419,-110.216185 C105.088268,-109.741342 104.481627,-109.578339 104,-109.856406 C103.521707,-110.132549 103.360156,-110.748163 103.630368,-111.216185 L113.320508,-128 L115.629909,-128 Z M128,-113.320508 L111.216185,-103.630368 C110.741342,-103.356217 110.134474,-103.518373 109.856406,-104 C109.580264,-104.478293 109.748163,-105.092206 110.216185,-105.362419 L128,-115.629909 L128,-113.320508 Z M48,-96 C48,-96.5522847 48.4523621,-97 48.9927864,-97 L79.0072136,-97 C79.5555144,-97 80,-96.5561352 80,-96 C80,-95.4477153 79.5476379,-95 79.0072136,-95 L48.9927864,-95 C48.4444856,-95 48,-95.4438648 48,-96 Z M54.4307806,-120 C54.706923,-120.478293 55.3225377,-120.639844 55.7905589,-120.369632 L81.7838153,-105.362419 C82.2586577,-105.088268 82.4216611,-104.481627 82.1435935,-104 C81.8674512,-103.521707 81.2518365,-103.360156 80.7838153,-103.630368 L54.7905589,-118.637581 C54.3157165,-118.911732 54.152713,-119.518373 54.4307806,-120 Z M104,-82.1435935 C104.478293,-82.4197359 105.092206,-82.2518365 105.362419,-81.7838153 L120.369632,-55.7905589 C120.643783,-55.3157165 120.481627,-54.7088482 120,-54.4307806 C119.521707,-54.1546382 118.907794,-54.3225377 118.637581,-54.7905589 L103.630368,-80.7838153 C103.356217,-81.2586577 103.518373,-81.865526 104,-82.1435935 Z M96,-80 C96.5522847,-80 97,-79.5476379 97,-79.0072136 L97,-48.9927864 C97,-48.4444856 96.5561352,-48 96,-48 C95.4477153,-48 95,-48.4523621 95,-48.9927864 L95,-79.0072136 C95,-79.5555144 95.4438648,-80 96,-80 Z M88,-82.1435935 C88.4782926,-81.8674512 88.6398444,-81.2518365 88.3696322,-80.7838153 L73.3624186,-54.7905589 C73.0882682,-54.3157165 72.4816272,-54.152713 72,-54.4307806 C71.5217074,-54.706923 71.3601556,-55.3225377 71.6303678,-55.7905589 L86.6375814,-81.7838153 C86.9117318,-82.2586577 87.5183728,-82.4216611 88,-82.1435935 Z M82.1435935,-88 C82.4197359,-87.5217074 82.2518365,-86.9077936 81.7838153,-86.6375814 L55.7905589,-71.6303678 C55.3157165,-71.3562174 54.7088482,-71.5183728 54.4307806,-72 C54.1546382,-72.4782926 54.3225377,-73.0922064 54.7905589,-73.3624186 L80.7838153,-88.3696322 C81.2586577,-88.6437826 81.865526,-88.4816272 82.1435935,-88 Z M1.30626177e-08,-41.9868843 L15.0170091,-57.9923909 L20.7983821,-52.9749272 L44.7207091,-81.2095939 L73.4260467,-42.1002685 L85.984793,-56.6159488 L104.48741,-34.0310661 L127.969109,-47.4978019 L127.969109,7.99473128e-07 L1.30626177e-08,7.99473128e-07 L1.30626177e-08,-41.9868843 Z M96,-84 C102.627417,-84 108,-89.372583 108,-96 C108,-102.627417 102.627417,-108 96,-108 C89.372583,-108 84,-102.627417 84,-96 C84,-89.372583 89.372583,-84 96,-84 Z"></path>
                <path d="M16,18 L112,18 C113.104569,18 114,17.1045695 114,16 C114,14.8954305 113.104569,14 112,14 L16,14 C14.8954305,14 14,14.8954305 14,16 C14,17.1045695 14.8954305,18 16,18 L16,18 Z M16,35 L112,35 C114.209139,35 116,33.209139 116,31 C116,28.790861 114.209139,27 112,27 L16,27 C13.790861,27 12,28.790861 12,31 C12,33.209139 13.790861,35 16,35 L16,35 Z M16,56 L112,56 C115.313708,56 118,53.3137085 118,50 C118,46.6862915 115.313708,44 112,44 L16,44 C12.6862915,44 10,46.6862915 10,50 C10,53.3137085 12.6862915,56 16,56 L16,56 Z M16,85 L112,85 C117.522847,85 122,80.5228475 122,75 C122,69.4771525 117.522847,65 112,65 L16,65 C10.4771525,65 6,69.4771525 6,75 C6,80.5228475 10.4771525,85 16,85 L16,85 Z M16,128 L112,128 C120.836556,128 128,120.836556 128,112 C128,103.163444 120.836556,96 112,96 L16,96 C7.163444,96 0,103.163444 0,112 C0,120.836556 7.163444,128 16,128 L16,128 Z"></path>
            </g>
        </svg>)""";

static constexpr std::string_view engine_svg = R"""(<?xml version="1.0" encoding="utf-8"?>
        <svg version="1.1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" height="22pt" width="22pt" viewBox="0 0 128 128">
            <g>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    752.029,41.568 956.716,154.044 1078.312,466.14 1069.192,803.569 1013.461,1138.971 941.516,1425.735 691.231,1587.863
                    541.263,1626.368 394.335,1579.756 152.156,1403.442 93.385,1117.692 49.813,785.33 49.813,461.074 182.555,156.071
                    384.202,42.581 568.622,26.368 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    384.202,42.581 414.964,248.458 568.622,26.368 709.613,248.092 752.029,41.568 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    182.555,156.071 414.964,248.458 709.613,248.092 558.027,465.506 414.964,248.458 198.53,391.325 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    198.53,391.325 558.027,465.506 232.2,605.036 198.53,391.325 49.813,461.074 124.388,677.131 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    928.627,395.044 887.501,611.422 558.027,465.506 928.627,395.044 997.569,690.428 1078.312,466.14 928.627,395.044
                    956.716,154.044 709.613,248.092 928.627,395.044 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    152.156,1403.442 244.54,1288.834 394.335,1579.756 545.501,1408.744 691.231,1587.863 851.732,1305.53 941.516,1425.735 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    93.385,1117.692 244.54,1288.834 232.906,988.803 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    851.732,1305.53 1013.461,1138.971 872.909,1007.048 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    444.159,1336.41 345.479,1295.118 462.327,1296.761 631.991,1299.197 646.266,1344.339 749.557,1305.972 693.258,1121.254
                    649.739,1250.433 749.557,1305.972 631.991,1299.197 649.739,1250.433 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    462.327,1296.761 547.701,1248.998 631.991,1299.197 545.163,1342.797 462.327,1296.761 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    649.739,1250.433 547.701,1248.998 447.386,1243.62 345.479,1295.118 405.91,1114.969 462.327,1296.761 444.159,1336.41
                    545.163,1342.797 646.266,1344.339 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    405.91,1114.969 512.603,1247.116 547.701,1133.554 452.079,1131.125 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    405.91,1114.969 484.878,1092.831 547.701,1133.554 548.606,1093.977 612.82,1093.977 646.266,1133.554 581.316,1248.998 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    693.258,1121.254 612.82,1093.977 547.701,1133.554 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    512.603,1247.116 452.079,1131.125 484.878,1092.831 548.606,1093.977 554.53,863.335 612.82,1093.977 611.794,884.229 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    124.388,677.131 232.2,605.036 231.979,646.201 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    124.388,677.131 49.813,785.33 167.363,785.33 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    232.2,605.036 444.375,680.397 231.979,646.201 295.764,724.245 124.388,677.131 244.996,750.194 167.363,785.33 232.906,988.803
                    235.342,810.384 167.363,785.33 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    670.327,685.448 887.501,611.422 997.569,690.428 1069.192,803.569 952.577,797.505 997.569,690.428 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    444.375,680.397 365.179,752.211 295.764,724.245 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    558.027,465.506 670.327,685.448 560.247,687.933 558.027,465.506 444.375,680.397 560.247,687.933 533.589,743.765
                    444.375,680.397 447.632,790.775 533.589,743.765 554.53,863.335 493.745,882.151 533.589,743.765 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    887.501,611.422 888.615,654.184 997.569,690.428 870.523,763.386 818.385,732.071 997.569,690.428 888.615,654.184
                    818.385,732.071 670.327,685.448 888.615,654.184 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    818.385,732.071 664.773,797.81 670.327,685.448 752.727,759.996 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    670.327,685.448 573.535,741.951 560.247,687.933 	"/>
                <polygon fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    573.535,741.951 664.773,797.81 611.794,884.229 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    870.523,763.386 952.577,797.505 872.909,1007.048 693.258,1121.254 851.732,1305.53 749.557,1305.972 691.231,1587.863
                    646.266,1344.339 545.501,1408.744 545.163,1342.797 547.701,1299.79 547.701,1248.998 547.701,1133.554 646.266,1133.554
                    693.258,1121.254 581.316,1248.998 547.701,1133.554 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    49.813,785.33 232.906,988.803 405.91,1114.969 244.54,1288.834 345.479,1295.118 394.335,1579.756 444.159,1336.41
                    545.501,1408.744 541.263,1626.368 541.263,1626.368 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    235.342,810.384 265.727,815.839 265.729,743.742 339.483,745.792 335.986,814.849 265.727,815.839 	"/>

                    <line fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" x1="244.996" y1="750.194" x2="295.764" y2="724.245"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    573.535,741.951 554.53,863.335 611.794,884.229 641.678,1037.551 664.773,797.81 736.807,820.817 773.337,825.163
                    847.962,824.482 952.577,797.505 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    335.986,814.849 378.8,811.943 447.632,790.775 458.903,1037.895 405.91,1114.969 303.92,814.698 232.906,988.803 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    378.8,811.943 458.903,1037.895 493.745,882.151 447.632,790.775 365.179,752.211 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    773.337,825.163 773.337,752.7 852.385,752.7 852.385,824.482 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    875.006,816.415 872.909,1007.048 1069.192,803.569 	"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    872.909,1007.048 805.794,823.835 693.258,1121.254 641.678,1037.551 805.794,823.835 	"/>

                    <line fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" x1="736.807" y1="820.817" x2="641.678" y2="1037.551"/>
                <polyline fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" points="
                    493.745,882.151 484.878,1092.831 458.903,1037.895 300.054,819.715 	"/>

                    <line fill="none" stroke="#000000" stroke-width="19.0665" stroke-linejoin="round" stroke-miterlimit="3" x1="554.53" y1="863.335" x2="484.878" y2="1092.831"/>
            </g>
        </svg>)""";*/

SDLApp::SDLApp()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    window.reset(SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, SDL_WINDOW_SHOWN));
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return;
    }
    /*window_surface.reset(SDL_GetWindowSurface(window.get()));
    if (!window_surface) {
        printf("Unable to create window surface! SDL Error: %s\n", SDL_GetError());
        return;
    }*/
    //renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_ACCELERATED));
    renderer.reset(SDL_CreateRenderer(window.get(), -1, SDL_RENDERER_SOFTWARE));
    if (!renderer) {
        printf("Unable to create renderer! SDL Error: %s\n", SDL_GetError());
        return;
    }
    // no special flags needed for SVG
    /*if (!IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG) {
        printf("SDL_Image cannot be initialized! IMG Error: %s\n", IMG_GetError());
        return;
    }*/
    for (auto& svgpair : { std::pair{muehle_board_svg, &board_surface},
         /*std::pair{back_svg, &back_surface},
         std::pair{cancel_svg, &cancel_surface},
         std::pair{delete_svg, &delete_surface},
         std::pair{depth_svg, &depth_surface},
         std::pair{engine_svg, &engine_surface}*/ }) {
        SDL_Surface* loaded_img = IMG_Load_RW(SDL_RWFromConstMem(svgpair.first.data(), svgpair.first.size()), 0);
        if (!loaded_img) {
            printf("Unable to load svg %s! IMG Error: %s\n", svgpair.first.data(), IMG_GetError());
            return;
        }
        //*(svgpair.second) = SDL_ConvertSurface(loaded_img, window_surface->format, 0);
        svgpair.second->reset(loaded_img);
        //SDL_FreeSurface(loaded_img);
        if (!(svgpair.second->get())) {
            printf("Unable to load svg %s! SDL Error: %s\n", svgpair.first.data(), SDL_GetError());
            return;
        }
    }
    IMG_Quit();
    //current_surface = board_surface.get();
    running = true;
}

SDLApp::~SDLApp()
{
    window_surface.reset();
    SDL_Quit();
}

int SDLApp::exec()
{
    while (running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            processEvents(&event);
        }
        render();
    }
    return 0;
}

// private

void SDLApp::processEvents(SDL_Event* event)
{
    switch (event->type) {
    case SDL_QUIT:
        running = false;
        break;
    case SDL_KEYDOWN:
        switch (event->key.keysym.sym) {
        /*case SDLK_UP:
            current_surface = engine_surface.get();
            break;
        case SDLK_DOWN:
            current_surface = depth_surface.get();
            break;
        case SDLK_LEFT:
            current_surface = back_surface.get();
            break;
        case SDLK_RIGHT:
            current_surface = delete_surface.get();
            break;*/
        default:
            //current_surface = board_surface.get();
            break;
        }
        break;
    default:
        break;
    }
}

void SDLApp::render()
{
    SDL_SetRenderDrawColor(renderer.get(), 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(renderer.get());
    SDL_Rect fillRect = { window_width / 4, window_height / 4, window_width / 2, window_height / 2 };
    SDL_SetRenderDrawColor(renderer.get(), 0xff, 0x00, 0x00, 0xff);
    SDL_RenderFillRect(renderer.get(), &fillRect);
    SDL_Rect outlineRect = { window_width / 6, window_height / 6, window_width * 2 / 3, window_height * 2 / 3 };
    SDL_SetRenderDrawColor(renderer.get(), 0x00, 0xff, 0x00, 0xff);
    SDL_RenderDrawRect(renderer.get(), &outlineRect);
    SDL_SetRenderDrawColor(renderer.get(), 0x00, 0x00, 0xff, 0xff);
    SDL_RenderDrawLine(renderer.get(), 0, window_height / 2, window_width, window_height / 2 );
    SDL_SetRenderDrawColor(renderer.get(), 0xff, 0xff, 0x00, 0xff);
    for(int i = 0; i < window_height; i += 4) {
        SDL_RenderDrawPoint(renderer.get(), window_width / 2, i);
    }
    SDL_RenderPresent(renderer.get());
    if (current_surface) {
        /*SDL_Rect stretchRect;
        stretchRect.x = 0;
        stretchRect.y = 0;
        stretchRect.w = window_width;
        stretchRect.h = window_height;
        SDL_BlitScaled( current_surface, NULL, window_surface, &stretchRect );*/
        SDL_BlitSurface(current_surface, nullptr, window_surface.get(), nullptr);
        SDL_UpdateWindowSurface(window.get());
    }
}
