# SolarIrrigation
This repository will have all firmware related resources pertaining to Team 8's Capstone project.


State Machine:
```mermaid
graph TD
  Start --> Initialization
  Initialization --> Standby
  Standby --> MPPT
  MPPT --> Standby
  Standby --> Charging
  Charging --> Standby
  Standby --> Error
  Charging --> Error
  Error --> Standby

  Start[Start]
  Initialization[Initialization]
  Standby[Standby]
  MPPT[Maximum Power Point Tracking]
  Charging[Battery Charging]
  Error[Error]

  subgraph SubGraphKey
    Start[Start]
    Initialization[Initialization]
    Standby[Standby]
    MPPT[Maximum Power Point Tracking]
    Charging[Battery Charging]
    Error[Error]
  end
```