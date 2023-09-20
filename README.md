# SolarIrrigation
This repository will have all firmware related resources pertaining to Team 8's Capstone project.


State Machine:
```mermaid
graph TD

subgraph FSM
    Start(Start)
    Initialization(Initialization)
    Standby(Standby)
    MPPTCharging(MPPT Charging)
    Error(Error)
    Sleep(Sleep)

    Start -->|Start| Initialization
    Initialization -->|Standby| Standby
    Standby -->|MPPT| MPPTCharging("isSunny == true")
    MPPTCharging -->|Standby| Standby("isSunny == false")
    Standby -->|Error| Error
    MPPTCharging -->|Error| Error
    Error -->|Standby| Standby
    Standby -->|Sleep| Sleep
    Sleep -->|Standby| Standby

    Standby -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")
    MPPTCharging -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")
    Error -->|reservoirFull ?| Standby("Enter Standby if reservoirFull condition is fulfilled")

    Standby -->|User input or environmental change| Standby("User input or environmental change")
    MPPTCharging -->|Continue MPPT tracking| MPPTCharging("Continue MPPT tracking")
    Error -->|Error handling and recovery| Error("Error handling and recovery")

    Standby -->|isSunny == false| Sleep("isSunny == false")
    Sleep -->|isSunny == true| Standby("isSunny == true")
end

end
```