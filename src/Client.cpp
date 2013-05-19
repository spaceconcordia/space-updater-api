//******************************************/
//  JAMG @ SPACE CONCORDIA 2013
//  Testing the client.
//
//******************************************/
#include "UpdaterClient.h"
int main(){
    UpdaterClient client("testApp1");
    UpdaterClient client2("testApp2");

    client.Connect();
    client2.Connect();

    client.Rollback("testApp1ToRollback");
    client2.Rollback("testApp2ToRollback");
    
    
    client.Disconnect();
    client2.Disconnect();
}
