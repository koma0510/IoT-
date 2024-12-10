#define MAX_BUFFER_SIZE 63

char recv_buffer[MAX_BUFFER_SIZE+1];
int n_recv_buffer=0;
boolean is_newline=false;

void setup(){
 Serial.begin(9600);
}

void loop(){
 while(Serial.available()){
   if(n_recv_buffer<MAX_BUFFER_SIZE){
     char c=Serial.read();
     
     if(c==0x7f){ // Back Space (BS)
        if(n_recv_buffer>0){
         Serial.print(c);
         recv_buffer[--n_recv_buffer]='\0';
        }
     } else if(0x20<=c && c<=0x7e){ // 通常文字の処理
       Serial.print(c);
       recv_buffer[n_recv_buffer++]=c;
     }else if(c==0x0d){ // 改行コード 0x0d
       is_newline=true;
       recv_buffer[n_recv_buffer]=0x00;
       Serial.println();
       break;
     }
   }else{
    Serial.println("Exceeded maxium length ... force new line");
     n_recv_buffer--;
     is_newline=true;
     recv_buffer[n_recv_buffer]=0x00;
     break;
   }
 }

 if(is_newline){
   int len=strlen(recv_buffer);
   Serial.print("len=");
   Serial.print(len);
   Serial.print(",\"");
   Serial.print(recv_buffer);
   Serial.println("\"");
   is_newline=false;
   n_recv_buffer=0;
 }
}
