char** convertKeys(char* key){
    int i;
    char *keya, *keyb, *keys[2];

    keya = (char*) malloc (32*sizeof(char));
    keyb = (char*) malloc (32*sizeof(char));
    keys = (char**) malloc (2*sizeof(char*));

    for(i = 0; i < 64; i++){
        if(i<32){
            keya[i] = key[i];
        }
        else{
            keyb[i-32] = key[i];
        }
    }

    keys[0] = keya;
    keys[1] = keyb;

    return keys;

}

int main(){
    return 0;
}
