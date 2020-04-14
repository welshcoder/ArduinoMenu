# Notes

## Memory

Memory figures for ATTiny85

2019.11.21 static tree with change and enable/disable control
```text
DATA:    [==        ]  24.6% (used 126 bytes from 512 bytes)
PROGRAM: [======    ]  63.5% (used 5202 bytes from 8192 bytes)
```

2019.11.25 - partial/minimal draw
```text
DATA:    [===       ]  25.6% (used 131 bytes from 512 bytes)
PROGRAM: [=======   ]  67.0% (used 5488 bytes from 8192 bytes)
```

2019.11.26 - partial/minimal draw (bug fixes)
```text
DATA:    [===       ]  25.6% (used 131 bytes from 512 bytes)
PROGRAM: [=======   ]  68.3% (used 5594 bytes from 8192 bytes)
```

2020.02.02 - joing dynamic and static API's
DATA:    [==        ]  23.6% (used 121 bytes from 512 bytes)
PROGRAM: [====      ]  35.3% (used 2894 bytes from 8192 bytes)

2020.02.07 - static&dynamic API's + PathRef length
DATA:    [==        ]  23.8% (used 122 bytes from 512 bytes)
PROGRAM: [=====     ]  54.2% (used 4444 bytes from 8192 bytes)

2020.02.08 - use panels, rewrite std::vector menu
DATA:    [==        ]  23.8% (used 122 bytes from 512 bytes)
PROGRAM: [======    ]  64.6% (used 5294 bytes from 8192 bytes)

2020.02.08 - toggle enable/disable on dynamic and static menus
DATA:    [==        ]  24.2% (used 124 bytes from 512 bytes)
PROGRAM: [=======   ]  69.4% (used 5684 bytes from 8192 bytes)

2020.02.09 rearranging files
DATA:    [==        ]  24.2% (used 124 bytes from 512 bytes)
PROGRAM: [======    ]  63.0% (used 5164 bytes from 8192 bytes)

2020.02.13 minimal draw (draw only changes)
DATA:    [==        ]  24.2% (used 124 bytes from 512 bytes)
PROGRAM: [=====     ]  45.5% (used 3730 bytes from 8192 bytes)

2020.02.18 review printing chain params
RAM:   [====      ]  35.9% (used 184 bytes from 512 bytes)
Flash: [=======   ]  65.7% (used 5380 bytes from 8192 bytes)

using regular param `fullPrint`
RAM:   [====      ]  35.9% (used 184 bytes from 512 bytes)
Flash: [=======   ]  65.7% (used 5380 bytes from 8192 bytes)

2020.02.22 tiny example with similar menu struct to compare with previous essays
RAM:   [===       ]  27.5% (used 141 bytes from 512 bytes)
Flash: [====      ]  38.3% (used 3136 bytes from 8192 bytes)