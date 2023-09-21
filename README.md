# SolarIrrigation
This repository will have all firmware related resources pertaining to Team 8's Capstone project.


State Machine:
```mermaid
stateDiagram-v2
        state Standby
        [*] --> Standby
        Standby --> MPPT: isSunny==true
        Standby --> Sleep: isSunny==false
        state MPPT {
        [*] --> Charging
        Charging --> NotCharging : batFull == true
        NotCharging --> Charging : batFull == false
        --
        [*] --> PumpIdle
        PumpIdle --> PumpWater : reservoirFull == false && suffBat == true
        PumpWater --> PumpIdle : reservoirFull == true
    }
    state Full <<join>>
    NotCharging --> Full
    PumpIdle --> Full
    Full --> Standby
    Sleep --> Standby: isSunny==true
        
        
    
    

```