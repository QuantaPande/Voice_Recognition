clc;
clear all;
close all;
t=serial('COM1', 'BaudRate', 19200, 'DataBits', 8, 'InputBufferSize', 1638);
fopen(t);
fwrite(t, 'A', 'async');
t.ReadAsyncMode = 'continuous';
receive=fread(t);
q=zeros(1,6,16000);
if(receive=='I')
    disp('Success');
    fwrite(t, 'A', 'async');
    q(1)=fread(t);
    q(1)=fft(q(1));
end
receive=fscanf(t);
if receive=='A'
    fprintf(t, 'A');
end
s=fread(t);
disp(s);
disp('successfully received');
ss=sscanf(s, '%d');
disp(ss);
count=0;
record=fft(ss);
for i=1:6
    y(i)=crosscorr(record,q(i));
    if ((1<y(i,21)) && (y(i,21)<0.8))
        readasync(t);
        z=fscanf(t, 1);
        if(z=='A')
            fprintf(t, 'A');
        end
        break;
    end
    count=count+1;
end
if(count==5)
    readasync(t);
    z=fscanf(t, 1);
    if(z=='A')
        fprintf(s, 'B');
    end
end
option=readasync(t);
if(option=='A')
        readsync(t);
        q(1)=fscanf(t, '%c');
        q(1)=sscanf(q(1), '%c');
        q(1)=fft(q(1));
        readasync(t);
        z=fscanf(t, 1);
        if(z=='A')
            fprintf(t, 'A');
        end
else if (option=='B')
            readasync(t);
            q(2)=fscanf(t, '%c');
            q(2)=sscanf(q(2), '%c');
            q(2)=fft(q(2));
            readasync(t);
            z=fscanf(t, 1);
            if(z=='A')
                fprintf(t, 'A');
            end
      end
end

fclose(t);
