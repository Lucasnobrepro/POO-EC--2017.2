#include <iostream>
#include "poo_repository.h"
#include "poo_controller.h"
#include "0_whatzap.h"
#include <vector>

string HELP = R"(help
)";


using namespace std;
class User;
class Chat;

class Registro{
public:
    User *user;
    int cont {0};
public:
    Registro(User *user = nullptr) {
        this->user = user;
    }
    void addCont(){
        cont += cont;
    }

    void zero(){
        cont = 0;
    }

};

class Zap{
    string id;
    string msg;
public:
    Zap(string id = "", string msg = "") {
        this->id = id;
        this->msg = msg;
    }
    friend class Chat;
};

class Chat{
protected:
    string id;
    map<string,Registro> users;
    vector<Zap> zaps;
public:
    Chat(string id = "") {
        this->id = id;
    }

    vector<string> getMgs(string idUser){
        vector<string> res;
        for(auto par : users)
            if(idUser == par.first){
                for(auto elem : zaps)
                    res.push_back(elem.msg);
                //for(int i = 0; i < par.second.cont; i++)
                    //if(zaps[zaps.size() - i -1].id != idUser)
                        //res.push_back(zaps[zaps.size() - i - 1].msg);


                par.second.zero();
            }
        return res;
    }

    //Recebe um Zap e o guadar na memoria do chat(Zaps, linha 43,
    //depois distribui a mensagem para todos os usuarios do derteminado
    //grupo menos pra quem enviou.
    void deliverZap(Zap zap){
        zaps.push_back(zap);
        for(auto& par : users)
            if(zap.id != par.first)
                par.second.addCont();
    }

    //Adiciona o usuario ao chat e o chat ao usuario, chama essa função
    //quando se cria um chat.
    bool addFirstuser(User* addirstUser);

    void newTalk(User * user1, User* user2);

    //Retorna todos os usuarios do chat, como um vector de string.
    vector<string> getUsers(){
        vector<string> res;
        for(auto& par : users)
            res.push_back(par.first);

        return res;
    }



    friend class User;
};

class Talk : public Chat{
public:
    Talk(string id = ""){
        this->id = id;
    }

    void leave(){
        throw "erro";
    }


    void invite(){

    }
};

class User{
public:
    string id;
    vector<Chat*> chats;

public:
    User(string id = ""){
        this->id = id;
    }

    //Retona um vector com o iD de todos os chats desse usuario.
    vector<string> getChats(){
        vector<string> res;
        for(auto *elem : chats)
            res.push_back(elem->id);
        return res;
    }

    //Convida um usuario para um chat, se o chat for um dos
    //chats desse usuario.
   void invite(string idChat, User* user){
        for(auto &elem : chats){
            if(elem->id == idChat){
                elem->users[user->id] = user;
                user->chats.push_back(elem);            }
        }

    }

    //Deixa um chat.
    void leave(string idChat){
        for(int i = 0; i < (int) chats.size(); i++){
            if(idChat == chats[i]->id){
                this->chats.erase(chats.begin() + i);
                chats[i]->users.erase(this->id);
            }

        }
    }

    //Envia uma mensagem para um grupo, ATENÇÃO na função deliverZap.
    void send(string msg, string IdChat){
        Zap zap(this->id, msg);
        for(auto &elem : chats)
            if(elem->id == IdChat){
                elem->deliverZap(zap);
            }
    }

    friend class Chat;

};



class Whatzap: public Controller {
public:
    Repository<Chat> r_chat;
    Repository<User> r_user;

    Whatzap():
    r_chat("chat"),
    r_user("user")
    {}
    string process(string line){
        auto ui = poo::split(line);
        auto cmd = ui[0];

        if(cmd == "help")
            return HELP;
        else if(cmd == "addUser"){//_user
            for(int i = 1; i < (int) ui.size(); i++)
                r_user.add(ui[i], User(ui[i]));
        }else if(cmd == "newGroup"){//_user _chat
              r_chat.add(ui[2], Chat(ui[2]))->addFirstuser(r_user.get(ui[1]));
        }else if(cmd =="newTalk"){//_user _user1 _talk
              r_chat.add(ui[3], Talk(ui[3]))->newTalk(r_user.get(ui[1]),r_user.get(ui[2]));
        }else if(cmd == "invite"){//_user _invited ... _invited _chat
            int size = ui.size();
            for(int i = 2; i < (size - 1); i++)
                r_user.get(ui[1])->invite(ui[size - 1], r_user.get(ui[i]));
        }else if(cmd == "leave"){//_user _chat
            r_user.get(ui[1])->leave(ui[2]);
        }else if(cmd == "zap"){//_user _chat _my_msg
            r_user.get(ui[1])->send(poo::join(poo::slice(ui, 3), " "), ui[2]);
        }else if(cmd == "ler"){//_user _chat
            return poo::join(r_chat.get(ui[2])->getMgs(ui[1]), "\n");
        }else if(cmd == "chats"){//_user
            return poo::join(r_user.get(ui[1])->getChats(), " ");
        }else if(cmd == "users"){//_chat
            return poo::join(r_chat.get(ui[1])->getUsers(), " ");
        }else if(cmd == "showUsers"){
            return poo::join(r_user.keys(), " ");
        }else if(cmd == "fim")
            return "";
        else
            return string("") + "comando invalido " + "[" + cmd + "]";
        return "done";
    }


};

int main()
{
    Whatzap whatzap;
    whatzap.commandLine();

    return 0;
}


bool Chat::addFirstuser(User *addirstUser){
    users[addirstUser->id] = addirstUser;
    addirstUser->chats.push_back(this);
    return true;
}

void Chat::newTalk(User *user1, User *user2){
    users[user1->id] = user1;
    users[user2->id] = user1;
    user1->chats.push_back(this);
    user2->chats.push_back(this);
}
