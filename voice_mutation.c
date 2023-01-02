#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define SWAP32(_x) ({ \
    u32 _ret = (_x); \
    (u32)((_ret << 24) | (_ret >> 24) | \
          ((_ret << 8) & 0x00FF0000) | \
          ((_ret >> 8) & 0x0000FF00)); \
  })

int get_size(u8* wav_buf)
{
    u32 size=0;
    size = *(u32*)(wav_buf + 0x04);
    return size+8;

}

//helper for wav modification
int size_fixer(u8* wav_buf,u32 len)
{
    //Size      offset:0x04    length:0x04  size=length-8
    //data_size offset:0x4A    length:0x04  data_size=length-0x4E
    u32 size=0,right_size=len-8;
    u32 data_size=0,right_data_size=len-0x4E;
    size = *(u32*)(wav_buf + 0x04);
    data_size = *(u32*)(wav_buf + 0x4A);

    // printf("%d\n%d\n",size,data_size);
    // printf("should be:%d\n%d\n",right_size,right_data_size);

    //check
    if (right_size!=size)
    {
        *(u32*)(wav_buf + 0x04)=right_size;
    }
    if (right_data_size!=data_size)
    {
        *(u32*)(wav_buf + 0x4A) = right_data_size;
    }
    
    // printf("%d\n%d\n",*(u32*)(wav_buf + 0x04),*(u32*)(wav_buf + 0x4A));

    
    return 0;
}
//frequency modification  return edit_distance
// TODO add or subtract a value || multiply a rate
int freq_mod(u8* wav_buf,u32 start,u32 width,u32 length,u16 value)
{
    u16 step = 2;
    // u32
    if (start+2*width>length)return 0;
    
    for (size_t i = 0; i < width; i+=step)
    {
        *(u16*)(wav_buf + start + 2*i) += value;
        //add edit distance
    }
    size_fixer(wav_buf,length);
    return 16*width;
}
//speed modification
//width is sample point offset,not memory offset!
//so that temp_len and i are also the sample point number
//start is the memory offset!
int speed_up(u8* wav_buf,u32 start,u32 width,u32 length,u16 rate)
{
    u32 temp_len = 0;
    u8 *temp_buf;
    // u8 *new_buf,*temp_buf;
    if (start+2*width>length)return 0;
    // new_buf = ck_alloc_nozero();
    temp_buf = malloc(sizeof(u8)*width);

    for (size_t i = 0; i < width; i+=rate,temp_len++)
    {
        *(u16*)(temp_buf + 2*temp_len) = *(u16*)(wav_buf+start+2*i);
    }
    // new_buf = malloc(sizeof(u8)*(length-2*width+2*temp_len));
    printf("%d\n",2*temp_len);
    //Head
    // memcpy(new_buf,wav_buf,start);
    //modified block
    memcpy(wav_buf+start,temp_buf,2*temp_len);
    //Tail
    memcpy(wav_buf+start+2*temp_len,wav_buf+start+2*width,length-start-2*width);
    //
    memset(wav_buf+length+2*temp_len-2*width,'\x00',width-2*temp_len);
    // free(wav_buf);
    // wav_buf = new_buf;
    size_fixer(wav_buf,length-2*width+2*temp_len);
    return (int)(16*width*(rate-1)/rate);

}


int speed_down(u8* wav_buf,u32 start,u32 width,u32 length,u16 rate)
{
    u32 temp_len = 0;
    u8 *new_buf;
    if (start+2*width>length)return 0;
    // new_buf = ck_alloc_nozero();
    new_buf = malloc(sizeof(u8)*(length+2*width*(rate-1)));

    //Head
    memcpy(new_buf,wav_buf,start);
    //modified block

    //middle
    for (size_t i = 0; i < width; i+=rate,temp_len++)
    {
        for (size_t j = 0; j < rate; j++)
        {
            *(u16*)(new_buf + start+2*(i*rate+j)) = *(u16*)(wav_buf+start+2*i);
        }
         
    }
    //Tail
    memcpy(wav_buf+start+2*width*rate,wav_buf+start+2*width,length-start-2*width);
    free(wav_buf);
    wav_buf = new_buf;
    size_fixer(wav_buf,length+2*width*(rate-1));
    return (int)(16*width*(rate-1));

}

//MIX

int voice_mix(u8* out_buf,u8* insert_buf,u32 start,u32 length,u32 insert_length,double ratio)
{

    for (size_t i = start; i < (length<insert_length?length:insert_length); i+=2)
    {
        //new = old + ratio*insert
        u16 temp = *(u16*)(insert_buf+i)
        *(u16*)(out_buf+i) += (u16)(ratio*(double)temp);
        
    }


    return 0;
}

int main(int argc, char const *argv[])
{
    u8 *wav_buf;
    u32 len=66958;
    FILE *fp;
    u32 new_size;
    fp=fopen("./0000.wav","rb");
    wav_buf = malloc(sizeof(u8)*len);
    for(int i=0;i<len;i++)
    {
        wav_buf[i]=fgetc(fp);
        // printf("%x",c);
        // WAV1[i]=c;
    }
    fclose(fp);
    // freq_mod(wav_buf,0x3910,0x1000,len,0xC000);
    // speed_up(wav_buf,0x3910,0x1000,len,2);
    // size_fixer(wav_buf,len);
    // printf("%d\n%d\n",*(u32*)(wav_buf + 0x04),*(u32*)(wav_buf + 0x4A));

    // printf("%d\n",get_size(wav_buf));
    fp=fopen("./0000_mod.wav","wb");
    for (size_t i = 0; i < get_size(wav_buf); i++)
    {
        fwrite(&wav_buf[i],sizeof(u8),1,fp);
    }
    
    // fwrite(wav_buf,strlen(wav_buf),1,fp);    
    fclose(fp);


    return 0;
}
