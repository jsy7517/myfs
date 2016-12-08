void mycpfrom(char * name){
    FILE * ifp;
    ifp = fopen(name,"rb");
    int size, full, remain, n=0;
    
    fseek(ifp, 0, SEEK_END);
    size = ftell(ifp);
    full = size/128;
    remain = size%128;
    rewind(ifp);
    
    for(int i=0;i<full;i++){
        for(int i =0; i<32;i++){
            n = find_free_datablock(sb -> freedatablock[i]);
            if(n){
                set_superblock_i(&(sb -> freeinode[i]));
                set_superblock_i(&(sb -> freedatablock[i]));
                break;
            }
        }
        
        fread(&db[n],128,1,ifp);
    }
    
    if(remain){
        for(int i =0; i<32;i++){
            n = find_free_datablock(sb -> freedatablock[i]);
            if(n){
                set_superblock_i(&(sb -> freeinode[i]));
                set_superblock_i(&(sb -> freedatablock[i]));
                break;
            }
        }
        fread(&db[n].data,remain,1,ifp);
        printf("%ld\n",ftell(ifp));
        //  set_inode_info(in,sb);
    }
}