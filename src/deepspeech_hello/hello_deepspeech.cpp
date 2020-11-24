#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include "deepspeech.h"

const char *model_path = "../deepspeech_models/deepspeech-0.9.1-models.pbmm";
const char *scorer_path = "../deepspeech_models/deepspeech-0.9.1-models.scorer";
const char *sample_path = "../../audio/sean_one_two_three_four_five.wav";

off_t fsize(const char *filename) {
    struct stat st; 

    if (stat(filename, &st) == 0)
        return st.st_size;

    return -1; 
}

typedef struct  WAV_HEADER{
    char                RIFF[4];        // RIFF Header      Magic header
    unsigned long       ChunkSize;      // RIFF Chunk Size  
    char                WAVE[4];        // WAVE Header      
    char                fmt[4];         // FMT header       
    unsigned long       Subchunk1Size;  // Size of the fmt chunk                                
    unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM 
    unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio                   
    unsigned long       SamplesPerSec;  // Sampling Frequency in Hz                             
    unsigned long       bytesPerSec;    // bytes per second 
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo 
    unsigned short      bitsPerSample;  // Number of bits per sample      
    char                Subchunk2ID[4]; // "data"  string   
    unsigned long       Subchunk2Size;  // Sampled data length    

}wav_hdr; 


int main(int argc, char**argv)
{
	ModelState *model_state;
	int rc;
	if ((rc = DS_CreateModel(model_path, &model_state)) == 0)
	{
		int sample_rate = DS_GetModelSampleRate(model_state);

		printf("created model! Sample Rate: %d\n", sample_rate);

		if ((rc=DS_EnableExternalScorer(model_state, scorer_path)) == 0)
		{
			printf("external scorer enabled\n");
		}


		if ( (rc = DS_AddHotWord(model_state, "five", 1000.5f)) == 0)
		{
			printf("added hot word\n");
		}
		else
		{
			printf("hotword failed: %x\n",rc);
		}
		

		unsigned int buffer_size = fsize(sample_path);
		const short* buffer = (short*)malloc(buffer_size);
		
		FILE *pf = fopen(sample_path, "rb");
		fread((char *)buffer, 1, buffer_size, pf);

		unsigned int number_of_samples = (buffer_size-sizeof(WAV_HEADER)) / 2;	// audacity saved as 16bit signed PCM
		const short * wav_data = buffer+sizeof(WAV_HEADER);

		printf("number of samples:%d\n", number_of_samples);

		char *text =  DS_SpeechToText(model_state,
                      wav_data,
                      number_of_samples);	

		printf("text: %s\n", text);

		Metadata * meta = DS_SpeechToTextWithMetadata(model_state,
                                      wav_data,
                                      number_of_samples,
                                      10);
		printf("meta returned %d transcripts\n", meta->num_transcripts);
		for (int i = 0; i < meta->num_transcripts; i++)
		{
			printf("%d: %2.2f", i, meta->transcripts[i].confidence);
			for (int j = 0; j < meta->transcripts[i].num_tokens; j++)
			{
				printf("%s", meta->transcripts[i].tokens[j].text);
			}
			printf("\n");
		}


		StreamingState *stream_state;
		if ((rc = DS_CreateStream(model_state, &stream_state)) == 0)
		{
			printf("created stream\n");

			// feed size of 20ms is 20/1000 2/100 of a second
			// sample rate is 16000samples/second
			// so feed 320 samples at a time with a 20ms wait between samples

			int current_sample = 0;
			int samples_submitted = 0;
			while (current_sample < number_of_samples)
			{
				int samples_to_submit = std::min<int>(number_of_samples - current_sample, 320);
				 DS_FeedAudioContent(stream_state,
                         &wav_data[current_sample],
                         samples_to_submit);
				
				samples_submitted += samples_to_submit;

				current_sample+= samples_to_submit;

				char *decode = DS_IntermediateDecode(stream_state);
				printf("%s\n", decode);


				usleep(20*1000);



			}
			printf("submitted %d\n", samples_submitted);
			char *decode =  DS_FinishStream(stream_state);
			printf("final: %s\n", decode);


		}


		// todo free the pointers
	}
	else 
	{
		printf("DS_CreateModel failed: %d\n", rc);
	}


	
		



}
