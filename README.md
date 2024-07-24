# Totem Arduino library

Single Arduino library containing header files for external control of Totem products.

```arduino
// Control over BLE
#include <TotemBLE.h>              // Discover Totem boards (scan)
#include <TotemMiniControlBoard.h> // Connect Mini Control Board
#include <TotemRoboBoardX3.h>      // Connect RoboBoard X3
#include <TotemRoboBoardX4.h>      // Connect RoboBoard X4
// Control over Serial
#include <TotemLabBoard.h>         // Interface Mini Lab
```

## BLE Boards

- Establish a BLE connection.
- Control motors and other features.
- Send user defined values and strings to RoboBoard.
- Library runs on any ESP32 (BLE capable) Arduino board.

| Board name | Description | Photo |
| --- | --- | --- |
| [Mini Control Board](https://docs.totemmaker.net/modules/mini-control-board/) | Remote robot control board | <a href="https://totemmaker.net/product/x3-fbi-board-bluetooth-motor-controller/"><img src="https://docs.totemmaker.net/assets/images/photo/mini-board-x3-v1.5v.2.0-photo.jpg" width="120"></a> |
| [RoboBoard X3](https://docs.totemmaker.net/roboboard-x3/) | Robot development board | <a href="https://totemmaker.net/product/roboboard-x3-programmable-robot-development-board/"><img src="https://docs.totemmaker.net/assets/images/photo/roboboard-x3-v3.1-card.jpg" width="120"></a> |
| [RoboBoard X4](https://docs.totemmaker.net/roboboard-x4/) | Robot development board | <a href="https://totemmaker.net/product/roboboard-x4-power-adapter-battery/"><img src="https://docs.totemmaker.net/assets/images/photo/roboboard-x4-v1.1-photo.jpg" width="120"></a> |

## Mini Lab

Manage and watch over LabBoard from any Arduino board (not limited to [TotemDuino](https://docs.totemmaker.net/totemduino/)).
| Board name | Description | Photo |
| --- | --- | --- |
| [LabBoard](https://docs.totemmaker.net/labboard/) | Measurement board for Mini Lab | <a href="https://totemmaker.net/product/totem-mini-lab/"><img src="https://docs.totemmaker.net/assets/images/photo/labboard-v2.2-photo.jpg" width="120"></a> |

## Documentation

Visit [docs.totemmaker.net/modules](https://docs.totemmaker.net/modules/)

## Contacts

To report issue, ask for feature or general discussion:

* Create [Github issue](https://github.com/totemmaker/TotemArduino/issues/new)
* Discuss in our [forum.totemmaker.net](https://forum.totemmaker.net)
* Contact our support [docs.totemmaker.net/support](https://docs.totemmaker.net/support/)