# SolarIrrigation
This repository will have all firmware related resources pertaining to Team 8's Capstone project.


State Machine:
```mermaid
scale 1200 height
graph TD

subgraph FSM
    [*] --> Start
    Start(Start)
    Initialization(Initialization)
    Standby(Standby)
    MPPT(Maximum Power Point Tracking)
    Charging(Battery Charging)
    Error(Error)
    Sleep(Sleep)

    Start -->|Start| Initialization
    Initialization -->|Standby| Standby
    Standby -->|MPPT| MPPT
    MPPT -->|Standby| Standby
    Standby -->|Charging| Charging
    Charging -->|Standby| Standby
    Standby -->|Error| Error
    Charging -->|Error| Error
    MPPT -->|Error| Error
    Error -->|Standby| Standby
    Standby -->|Sleep| Sleep
    Sleep -->|Standby| Standby

    Standby -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")
    MPPT -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")
    Charging -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")
    Error -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")

    Standby -->|User input or environmental change| Standby("User input or environmental change")
    MPPT -->|Continue MPPT tracking| MPPT("Continue MPPT tracking")
    Charging -->|Charging battery| Charging("Charging battery")
    Error -->|Error handling and recovery| Error("Error handling and recovery")

    Standby -->|isSunny == false| Sleep("isSunny == false")
    Sleep -->|isSunny == true| Standby("isSunny == true")
end
```