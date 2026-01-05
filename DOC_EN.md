# Usage Documentation
## Blueprint Node Descriptions
- **OpenSession**  
Parameters: None  
Description: Open a session  
Notes: When opening a level, you must add `listen`, otherwise it will not take effect
---
- **CloseSession**  
Parameters: None  
Description: Close the session  
Notes: None
---
- **SetSessionPassword**  
Parameters: `string` Password  
Description: Set the session password  
Notes: If left empty, it means there is no password and anyone can connect
---
- **SetSessionProperty**  
Parameters: `string` Property  
Description: Set the session property  
Notes: Usually used for room names; not recommended to change after **OpenSession**
---
- **SetSessionPropertyAppend**  
Parameters: `string` PropertyAppend  
Description: Set additional session properties  
Notes: Commonly used for map, player count, etc.; supports dynamic modification after **OpenSession**
---
- **BindOnAddrUpdate**  
Parameters: `Delegate`  
Description: Bind an event to “when Addr is received”  
Notes: None
---
- **UnbindOnAddrUpdate**  
Parameters: None  
Description: Unbind the event “when Addr is received”  
Notes: Be careful not to unbind when nothing is bound
---
- **BindOnSessionListUpdate**  
Parameters: `Delegate`  
Description: Bind an event to “when the session list is received”  
Notes: None
---
- **UnbindOnSessionListUpdate**  
Parameters: None  
Description: Unbind the event “when the session list is received”  
Notes: Be careful not to unbind when nothing is bound
---
- **FlushFRPSessionList**  
Parameters: None  
Description: Refresh the session list  
Notes: Use together with **BindOnSessionListUpdate**
---
- **AskForAddress**  
Parameters: `string` ID | `string` Password  
Description: Request Addr based on session ID and password  
Notes: Password can be left empty
---
- **SetAppAddress**  
Parameters: `int` Port  
Description: Set the App port  
Notes: Must be consistent with the server
---
- **SetMsgAddress**  
Parameters: `int` Port  
Description: Set the Msg port  
Notes: Must be consistent with the server
---
- **SetBaseIP**  
Parameters: `string` IP  
Description: Set the server IP  
Notes: Must be consistent with the server
---
- **ConnectSession**  
Parameters: `string` Addr  
Description: Connect to the session after successfully requesting Addr  
Notes: Use together with **BindOnAddrUpdate**
---
