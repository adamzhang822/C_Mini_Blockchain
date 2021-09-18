A mini blockchain for fake cyrptocurrency implemented in C.

Part 1 implements the following:
- Structs for  addresses, transactions, blocks, and the blockchain
- Withdrawing from and depositing into accounts 
- Rewarding miners by depositng coins into their accounts
- Detecting and tracking double spending (when a user tries to withdraw more than they own)

Part 2 implements the following:
- Deanonymizing the blockchain by trying to cluster accounts that may belong to the same user through tracking transaction history 
