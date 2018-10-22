#include <iostream>
#include <string>
#include <vector>

using namespace std;

// FUNCOES;
void gerar_tabelas();
vector<int> expanda_chave(vector<int> entrada);
vector<int> chave_main(vector<int> entrada, int iter);
vector<int> rotacione(vector<int> entrada);
int sbox(int entrada);
int ginv_mult(int num);
int rcon(int num);

// VARIAVEIS;
int tabela_al[256], tabela_l[256];

int main(void){
  gerar_tabelas();
  vector<int> senha, senhaexp;
  cout << "Digite a senha de criptografia (16 caracteres).\n";
  for(int i = 0; i < 16; i++){
    int num;
    cin >> hex >> num;
    senha.push_back(num);
  }
  cout << "Gerando sub-chaves...\n";
  #if(DEBUG)
  cout << "A tabela LOG e a tabela ANTI-LOG é:\n";
  for(int i = 0; i < 255; i++){ cout << hex << tabela_l[i] << "\n"; }
  cout << "------------------------\n";
  for(int i = 0; i < 255; i++){ cout << hex << tabela_al[i] << "\n"; }
  #endif
  senhaexp = expanda_chave(senha);
  cout << "As senhas sao:\n";
  for(int i = 0; i < 11; i++){
    for(int j = 0; j < 16; j++){
      cout << senha[i+j] << " ";
    }
    cout << "\n";
  }
  return 0;
}

void gerar_tabelas(){
  unsigned char a = 1;
  unsigned char d;
  for(unsigned char i = 0; i < 255; i++){
    tabela_al[i] = a;
    d = (a & 0x80);
    a = (a << 1);
    if(d == 0x80) a = (a ^ 0x1b);
    a = (a ^ tabela_al[i]);
    tabela_l[tabela_al[i]] = i;
  }
  tabela_al[255] = tabela_al[0];
  tabela_l[0] = 0;
  tabela_l[1] = 0xff;
}


vector<int> expanda_chave(vector<int> entrada){
  vector<int> temp; // Variavel temporária
  int qtd = 16; // Quantidade de bytes que eu tenho até agora. 16 pq as 16 primeiras é o usuário que da.
  int iter = 1; // Valor de iteração do RCON.

  while(qtd < 176){
    for(int i = 0; i < 4; i++) temp.push_back(entrada[qtd-4+iter]); // Copia os ultimo bloco de 4 chars para um vetor auxiliar
    if(qtd % 16 == 0){ // A cada 4 blocos de 4 bytes
      chave_main(temp, iter);
      iter++;
    }

    for(int i = 0; i < 4; i++) entrada.push_back(entrada[qtd++ -16] ^ temp[i]);
    temp.clear();
  }
  return entrada;
}

vector<int> chave_main(vector<int> entrada, int iter){
  entrada = rotacione(entrada);
  for(int i = 0; i < 4; i++) entrada[i] = sbox(entrada[i]);
  entrada[0] ^= rcon(iter);
  return entrada;
}

vector<int> rotacione(vector<int> entrada){
  vector<int> aux;
  for(int i = 0; i < 4; i++) aux[i] = entrada[(i+1)%4];
  return aux;
}

int sbox(int entrada){
  int a, b;
  a = b = ginv_mult(entrada);
  for(int i = 0; i < 4; i++){
    a = ((a << 1)%255) | ((a >> 7)%255);
    b ^= a;
  }
  b ^= 99;
  return b%255;
}

// Calcula o inverso multiplicativo no gampo galoisiano, através de uma tabela já
// pré calculada.
int ginv_mult(int num){
  if(num == 0) return 0;
  return tabela_al[(255 - tabela_l[num])];
}

int rcon(int num){
  unsigned char c = 1, b;
  if(num == 0) return 0;
  while(num != 1){
    b = c & 0x80;
    c = c << 1;
    if(b == 0x80) c ^= 0x1b;
    num --;
  }
  return (int)c;
}
