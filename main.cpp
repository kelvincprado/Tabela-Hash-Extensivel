#include <iostream>
#include <vector>
#include <map>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include <cstdlib>
using namespace std;

//Criando estruturas de Bucket e Diretorio
struct Bucket{
    vector<int> ProfundidadeLocal;
    int QtdDeBuckets;
    map<int, vector<int>> bucket;
    int tamanhoBucket = 4;
};

struct Diretorio{
    int ProfundidadeGlobal;
    int QtdCelulasDoDiretorio;
    vector<int> IdDiretorios;
};

void AdicionarBucket(Bucket *&buc, int indice, int chave, Diretorio *&dir);

//Procedimento que inicializa o diretorio e os buckets com valores predefinidos.
void inicializarDiretorioEBucket(Diretorio *&dir, Bucket *&buc){
    dir = new Diretorio();
    dir->ProfundidadeGlobal = 2;
    dir->QtdCelulasDoDiretorio = 4;
    for (int i=0; i<dir->QtdCelulasDoDiretorio; i++){
        dir->IdDiretorios.push_back(i);
    }

    buc = new Bucket();
    buc->QtdDeBuckets = 4;
    for(int i=0; i<buc->QtdDeBuckets; i++){
        buc->ProfundidadeLocal.push_back(2);
    }
}

//Procedimento que mostra os buckets
void mostrar(Diretorio *&dir, Bucket *&buc){
    for(map<int, vector<int> >::iterator ii=buc->bucket.begin(); ii!=buc->bucket.end(); ++ii){
       cout << (*ii).first << ": ";
       vector <int> VetorDasChaves = (*ii).second;
       for (int j=0; j<VetorDasChaves.size(); j++){
           cout << VetorDasChaves[j] << ", ";
       }
       cout << endl;
   }
}

//Verifica se existe uma chave ja existente
bool chave_repetida(Bucket *&buc, int chave){
    bool achou = false;
    for(map<int, vector<int> >::iterator ii=buc->bucket.begin(); ii!=buc->bucket.end(); ++ii){
       vector<int> VetorDasChaves = (*ii).second;
       for (int j=0; j<VetorDasChaves.size(); j++){
           if(VetorDasChaves[j] == chave){
                achou = true;
           }
       }
   }
   return achou;
}

//Faz o procedimento de encontrar uma chave
void busca_por_igualdade(Bucket *&buc, int chave){
    bool achou = false;
    for(map<int, vector<int> >::iterator ii=buc->bucket.begin(); ii!=buc->bucket.end(); ++ii){
       vector<int> VetorDasChaves = (*ii).second;
       for (int j=0; j<VetorDasChaves.size(); j++){
           if(VetorDasChaves[j] == chave){
                achou = true;
           }
       }
    }
    if (achou == true){
        cout << "Chave: " << chave << " encontrada!" << endl;
    }else{
        cout << "Chave não encontrada!" << endl;
    }
}

//Função de conversão de decimal para binario
string decimal_para_binario(int chave){
    string x;
    while (chave != 0){
        if (chave % 2 == 0){
            x += "0";
        }else{
            x += "1";
        }
        chave = chave / 2;
    }
    string resultado(x.rbegin(), x.rend()); //inverte a string deixando correta
    return resultado;
}

//Função que converte de binario para decimal
int binario_para_decimal(string n){
    string num = n;
    int valor_decimal = 0;
    int base = 1;

    int tam = num.length();
    for (int i = tam - 1; i >= 0; i--) {
        if (num[i] == '1')
            valor_decimal += base;
        base = base * 2;
    }
    return valor_decimal;
}

//Função que retorna os ultimos digitos de uma chave, para identificar qual será seu diretorio
string funcao_hash(int chave, Bucket *&buc, int indice = 2){
    string binario = decimal_para_binario(chave);
    string ultimosDigitos = "";

    for (int i=1; i<=buc->ProfundidadeLocal[indice]; i++){
        ultimosDigitos += binario[binario.length() - i];
    }
    string inversoDosUltimosDigitos(ultimosDigitos.rbegin(), ultimosDigitos.rend()); //inverte a string deixando correta
    return inversoDosUltimosDigitos;
}

//Procedimento que remove um numero do bucket
void remover_num(Bucket *&buc, Diretorio *&dir, int chave){
    if (chave_repetida(buc, chave)){ //Verifica se existe essa chave no bucket
        string chave_diretorio_bin = funcao_hash(chave, buc);
        int chave_diretorio_dec = binario_para_decimal(chave_diretorio_bin);
        //Encontra o diretorio que estara essa chave, e então remove a chave do bucket.
        for (auto i=buc->bucket[chave_diretorio_dec].begin(); i!=buc->bucket[chave_diretorio_dec].end(); ++i){
            if (chave == *i){
                buc->bucket[chave_diretorio_dec].erase(i);
                i--;
            }
        }
    }else{
        cout << "Não existe essa chave no bucket" << endl;
    }
}

//Devemos duplicar o tamanho do diretório sempre que há uma profundidade local maior do que a profundidade global
void DuplicarDiretorio(Diretorio *&dir){
    dir->ProfundidadeGlobal++;
    dir->QtdCelulasDoDiretorio *= 2;
    for(int i=dir->IdDiretorios.size(); i<dir->QtdCelulasDoDiretorio; i++){
        dir->IdDiretorios.push_back(i);
    }
}

//Esse procedimento fara uma redistribuição dos buckets apos sofrer overflow
void redistribuir(Bucket *&buc, int indice, int chave, vector<int>& vet, Diretorio *&dir){
    buc->bucket[indice].clear(); //limpara o bucket que sofreu overflow
    for (auto x:vet){ //para cada chave no vetor armazenando os valores que estava no bucket, fara uma nova inserção, considerando os novos valores de profundidade
        string chave_diretorio_bin = funcao_hash(x, buc, indice);
        int chave_diretorio_dec = binario_para_decimal(chave_diretorio_bin);
        buc->bucket[chave_diretorio_dec].push_back(x);
    }
    //fará a inserção da chave que fez sofrer o overflow
    string chave_diretorio_bin = funcao_hash(chave, buc, indice);
    int chave_diretorio_dec = binario_para_decimal(chave_diretorio_bin);
    if (buc->bucket[chave_diretorio_dec].size() < buc->tamanhoBucket){
        if (chave_repetida(buc, chave)){
            cout << "A chave " << chave << " já está inserida! Não pode inserir números duplicados" << endl;
        }else{
            buc->bucket[chave_diretorio_dec].push_back(chave);
        }
    }else{
        AdicionarBucket(buc, chave_diretorio_dec, chave, dir);
        if (buc->ProfundidadeLocal[chave_diretorio_dec] > dir->ProfundidadeGlobal){
            DuplicarDiretorio(dir);
        }
    }
}

//Procedimento que adiciona mais um bucket pois sofreu overflow
void AdicionarBucket(Bucket *&buc, int indice, int chave, Diretorio *&dir){
    //Atribui os novos valores ao bucket que sofreu overflow e ao novo bucket criado
    buc->ProfundidadeLocal.insert(buc->ProfundidadeLocal.begin() + indice, buc->ProfundidadeLocal[indice]++);
    buc->ProfundidadeLocal.erase(buc->ProfundidadeLocal.begin() + indice -1);
    buc->QtdDeBuckets++;
    buc->ProfundidadeLocal.push_back(buc->ProfundidadeLocal[indice]);

    vector<vector<int>> vec;
    for(map<int, vector<int>>::iterator Iter=buc->bucket.begin(); Iter!=buc->bucket.end(); ++Iter){
        vector <int> VetorDasChaves = (*Iter).second;
        vec.push_back(VetorDasChaves);
   }

   //cria um vetor armazenando os valores do bucket que sofreu overflow
    vector<int> vet;
    for (int i = 0; i < vec[indice].size(); i++){
        vet.push_back(vec[indice][i]);
    }

    redistribuir(buc, indice, chave, vet, dir);
}

//Faz a inserção de uma chave nos buckets
void inserir(Diretorio *&dir, Bucket *&buc, int chave){
    //calcula em qual diretorio a chave pertence
    string chave_diretorio_bin = funcao_hash(chave, buc);
    int chave_diretorio_dec = binario_para_decimal(chave_diretorio_bin);

    if (buc->bucket[chave_diretorio_dec].size() < buc->tamanhoBucket){ //verifica se tem tamanho no bucket para fazer a inserção da chave
        if (chave_repetida(buc, chave)){
            cout << "A chave " << chave << " já está inserida! Não pode inserir números duplicados" << endl;
        }else{
            buc->bucket[chave_diretorio_dec].push_back(chave);
        }
    }else{ //caso não tenha espaço, sofrera um overflow, adicionando um novo bucket.
        AdicionarBucket(buc, chave_diretorio_dec, chave, dir);
        if (buc->ProfundidadeLocal[chave_diretorio_dec] > dir->ProfundidadeGlobal){
            //Devemos duplicar o tamanho do diretório sempre que há uma profundidade local maior do que a profundidade global
            DuplicarDiretorio(dir);
        }
    }
}

void escolha_predefinida(){
    Diretorio *diretorio;
    Bucket *bucket;

    inicializarDiretorioEBucket(diretorio, bucket);

    inserir(diretorio, bucket, 1);
    inserir(diretorio, bucket, 5);
    inserir(diretorio, bucket, 4);
    inserir(diretorio, bucket, 12);
    inserir(diretorio, bucket, 32);
    inserir(diretorio, bucket, 16);
    inserir(diretorio, bucket, 21);
    inserir(diretorio, bucket, 10);
    inserir(diretorio, bucket, 15);
    inserir(diretorio, bucket, 7);
    inserir(diretorio, bucket, 19);
    inserir(diretorio, bucket, 13);
    mostrar(diretorio, bucket);

    cout << "\nInserindo valor 20, cujo diretorio seria o 00, porem está cheio e sofrera overflow, fazendo uma redistribuição e criando novo bucket." << endl;
    inserir(diretorio, bucket, 20);
    mostrar(diretorio, bucket);

    int buscar;
    cout << "\nDigite um número para buscar na tabela hash: ";
    cin >> buscar;
    cout << "Fazendo uma busca por igualdade." << endl;
    busca_por_igualdade(bucket, buscar);

    int remover_numero;
    cout << "\nDigite um número para remover da tabela hash: ";
    cin >> remover_numero;
    cout << "Removendo um número do bucket...." << endl;
    remover_num(bucket, diretorio, remover_numero);
    getchar();

    cout << "Mostrando Buckets depois da remoção." << endl;
    mostrar(diretorio, bucket);
    getchar();
}

void criar(){
    Diretorio *diretorioCriado;
    Bucket *bucketCriado;

    inicializarDiretorioEBucket(diretorioCriado, bucketCriado);

    int opc;
    while (opc != 5){
        cout << " ___________________________ " <<  endl;
        cout << "| Escolha uma opção:        |" << endl;
        cout << "| 1) Inserir número         |" << endl;
        cout << "| 2) Remover um número      |" << endl;
        cout << "| 3) Buscar número          |" << endl;
        cout << "| 4) Mostrar tabela Hash    |" << endl;
        cout << "| 5) Sair                   |" << endl;
        cout << "|___________________________|" << endl;
        cin >> opc;
        switch (opc) {
            case 1:
                int num;
                while(true){
                    cout << "Digite um numero para inserir na tabela Hash [-1 para sair]: ";
                    cin >> num;
                    if (num == -1){
                        break;
                    }
                    inserir(diretorioCriado, bucketCriado, num);
                }
                break;
            case 2:
                int numeropararemover;
                cout << "Digite o número que deseja remover: ";
                cin >> numeropararemover;
                remover_num(bucketCriado, diretorioCriado, numeropararemover);
                getchar();
                break;
            case 3:
                int buscarnumero;
                cout << "Digite o número que deseja buscar: ";
                cin >> buscarnumero;
                busca_por_igualdade(bucketCriado, buscarnumero);
                getchar();
                break;
            case 4:
                mostrar(diretorioCriado, bucketCriado);
                getchar();
                break;
            case 5:
                break;
            default:
                cout << "Digite a letra correta" << endl;
                getchar();
                break;
        }
    }
}

int main() {
    int opc;
    while (opc != 3){
        cout << " ___________________________ " <<  endl;
        cout << "| Escolha uma opção:        |" << endl;
        cout << "| 1) Escolha predefinida    |" << endl;
        cout << "| 2) Criar Hash             |" << endl;
        cout << "| 3) Sair                   |" << endl;
        cout << "|___________________________|" << endl;
        cin >> opc;
        switch(opc){
            case 1:
                escolha_predefinida();
                break;
            case 2:
                criar();
                break;
            case 3:
                break;
            default:
                cout << "Digite a letra correta" << endl;
                break;
        }
    }
    return 0;
}
