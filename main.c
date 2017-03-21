// currently pretty much just hacked out of the stmbl/term program

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <unistd.h> // for sleep function
#include <libserialport.h> // cross platform serial port lib

#define BUFSIZE 20000

struct sp_port *port;
struct sp_port **ports;
unsigned char buf_raw[BUFSIZE+1];
//unsigned char buf[1000];

void init_stmbl_port(struct sp_port *port);
void write_to_stmbl_port(struct sp_port *port, char message[]);
struct sp_port * get_stmbl_port ();
void read_from_stmbl_port(struct sp_port *port);


int main() {
    struct sp_port *port;
    enum sp_return error;

    port = get_stmbl_port();
    error = sp_open(port, SP_MODE_READ_WRITE);

    if (error == SP_OK){//port available and can be opened
    	init_stmbl_port(port);
//
//        write_to_stmbl_port(port, "fault0.reset = 1");
//        write_to_stmbl_port(port, "fault0.reset = 0");

        while (1) {
            //sleep(0.5); // can do something else in mean time
            read_from_stmbl_port(port);
        }
        sp_close(port);
    }
    else {
        printf("No serial devices detected\n");
    }

    if (ports) {
        sp_free_port_list(ports);
    }

    return 0;
}

void read_from_stmbl_port(struct sp_port *port){
    int ret;
    int i;
    int buf_count = 0;
    ret = sp_blocking_read(port, buf_raw, BUFSIZE, 1);
    if (ret > 0) {
        buf_raw[ret] = 0;
        for (i = 9; i<ret; ++i) {
            if (buf_raw[i] == 0xff) {
                break;
            }
            else {
                printf("%c",buf_raw[i]&0x7f);
            }
        }
    }
}

struct sp_port *get_stmbl_port () {
    char *descr;
    int found;
    enum sp_return error;
    char descr_stmbl[6] = "STMBL";
    error = sp_list_ports(&ports);
    if (error == SP_OK) {
        for (int i = 0; ports[i]; i++) {
            found = 1;
            descr = sp_get_port_description(ports[i]);
            for (int j = 0; j<5; ++j){
                if (descr[j] != descr_stmbl[j]) {
                    found = 0;
                }
            }
            if (found){
                printf("STMBL found\n");
                port = ports[i];
            }
            else {
                port = NULL;
            }
        }
    }
    return(port);
}

void init_stmbl_port(struct sp_port *port){
	sp_set_baudrate(port,38400);
	sp_set_bits(port, 8);
	sp_set_stopbits(port, 1);
	sp_set_parity(port, SP_PARITY_NONE);
	sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
	sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);

}

/*
void write_to_stmbl_port(struct sp_port *port, char message[]){
    size_t len = strlen(message);
    int ret1 = sp_nonblocking_write(port, message, len);
    int ret2 = sp_nonblocking_write(port, "\n", 1);
    if(ret1 != len || ret2!=1){
        printf("Error while sending");
    }
}
*/



//void ServoFrame::OnTimer(wxTimerEvent& evt){
//	int ret;
//	if(connected){
//        if(!txqueue.empty()){
//            int ret1 = sp_nonblocking_write(port, txqueue.front().c_str(), txqueue.front().length());
//            int ret2 = sp_nonblocking_write(port, "\n", 1);
//            if(ret1 != txqueue.front().length() || ret2!=1){
//                wxMessageBox( wxT("Error while sending"), wxT("Error"), wxICON_EXCLAMATION);
//                disconnect();
//            }
//            txqueue.pop();
//        }
//		ret = sp_nonblocking_read(port, buf, bufsize);
//		if(ret > 0){
//			buf[ret] = 0;
//			if(uhu->GetValue()){
//                for (int i=0; i<ret; i++) {
//                    if ((buf[i]>>7)) {
//                        values[0] = (float)(buf[i]-128-64)/64;
//                        drawpanel->plotvalue(values);
//                    }else{
//                        text->AppendText(wxString::FromAscii(buf[i] & 0x7f));
//                    }
//                }
//
//            }else if(stmbl->GetValue()){
//                for (int i=0; i<ret; i++){
//                    if(addr >= 0){
//                        values[addr++] = (buf[i]-128) / 128.0;
//                        if(addr == 8){
//                            drawpanel->plotvalue(values);
//                            addr = -1;
//                        }
//                    }else if (buf[i] == 0xff) {
//                        addr = 0;
//                    }else{
//                        text->AppendText(wxString::FromAscii(buf[i] & 0x7f));
//                    }
//                }
//            }
//        }else if(ret < 0){//disconnect on failure
//            disconnect();
//        }
//	}
//}



// using std::cout;
// using std::endl;
// using std::string;
// using std::to_string;
// 
// int ServoFrame::send(const string& s,bool h){
//     if(!connected)
//         connect();
//     if(connected){
//         if(h){//history
//             if((history.size()==0 || history.back() != s) && !s.empty()){
//                 history.push_back(s);
//             }
//             histpos = history.size();
//         }
//         txqueue.push(s);
//     }
//     return 1;
// }
// 
// void ServoFrame::connect(){
// // 	if(choose_port->IsEmpty())
// //         return;
// //    port = ports[choose_port->GetSelection()];
//     port = ports[choose_port->GetSelection()];
// 	if(sp_open(port, SP_MODE_READ_WRITE) == SP_OK){//port available and can be opened
// 		sp_set_baudrate(port,38400);
// 		sp_set_bits(port, 8);
// 		sp_set_stopbits(port, 1);
// 		sp_set_parity(port, SP_PARITY_NONE);
// 		sp_set_xon_xoff(port, SP_XONXOFF_DISABLED);
// 		sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
// 		connected = true;
// //		connectbutton->SetLabel(wxT("&Disonnect"));
// //		refresh->Disable();
// //		reset->Enable();
// //		choose_port->Disable();
// //      uhu->Disable();
// //      stmbl->Disable();
// //		textinput->SetFocus();
// //      timer->Start(50);
// 	}else{
// 		wxMessageBox( wxT("Cannot open port"), wxT("Error"), wxICON_EXCLAMATION);
// 		listports();
// 	}
// }
// 
// void ServoFrame::disconnect(){
// 	if(sp_close(port) == SP_OK){
// 		connected = false;
// 		connectbutton->SetLabel(wxT("&Connect"));
// 		refresh->Enable();
// 		reset->Disable();
// 		choose_port->Enable();
//         uhu->Enable();
//         stmbl->Enable();
//         timer->Stop();
// 	}
//     listports();
// }
// 
// void ServoFrame::listports(){
//     int selection = 0;
//     string description;
// 	if (ports) {
// 		sp_free_port_list(ports);
// 	}
// 	if(sp_list_ports(&ports) == SP_OK){
// 		wxString str;
// 		choose_port->Clear();
//         if(ports){
//             for (int i = 0; ports[i]; i++) {
//                 description = sp_get_port_description(ports[i]);
//                 choose_port->Append(description);
//                 if(description.find("STMBL") != std::string::npos){
//                     selection = i;
//                 }
//             }
//             choose_port->SetSelection(selection);
//             connectbutton->Enable();
//         }else{
//             connectbutton->Disable();
//         }
// 	}
// }
// 
// 
// // void ServoFrame::OnInput(wxCommandEvent& event){
// //     string s =string(textinput->GetValue().mb_str());
// //     send(s,true);
// // 	textinput->Clear();
// // }




