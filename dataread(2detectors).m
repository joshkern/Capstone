clear all
clc


dataport = serial('COM19', 'Baudrate', 115200, 'Timeout' , 15000, 'InputBufferSize', 51200);
fopen(dataport);
fsanf(dataport);
msg = fread(dataport);
fclose(dataport);
%%
data = dec2bin(msg);

data = reshape(data',1,length(msg)*8);
%% do the right thing
count =0;
for i =1:length(data)
    if(count>=16 && data(i)==dec2bin(0))
        break
    end
    if(data(i)==dec2bin(1))
        count = count+1;
    else
        count=0;
    end
    
end
%%
processeddata = data(i-16:length(data));
processeddata = processeddata(1:length(processeddata)-rem(length(processeddata),144));
pd = reshape(processeddata,16,length(processeddata)/16)';
%%

Vc=3.3;
acc=12;
division = 3.3/(2^12);
pd_dec = bin2dec(pd);
pd_dec_v = pd_dec*division;
x=reshape(pd_dec_v,9,(length(pd_dec_v))/9)';
fs=500;
ts = 1/fs;
t = ts:ts:ts*length(x);
pd1 = [x(:,2),x(:,4),x(:,6),x(:,8)];
pd2 = [x(:,3),x(:,5),x(:,7),x(:,9)];
hold on
plot(t,x(:,2),'.',t,x(:,3),'.',t,x(:,4),'.',t,x(:,5),'.');
%%
order=1;
frame =17;
fd1(:,1)=sgolayfilt(pd1(:,1),order,frame);
fd1(:,2)=sgolayfilt(pd1(:,2),order,frame);
fd1(:,3)=sgolayfilt(pd1(:,3),order,frame);
fd2(:,1)=sgolayfilt(pd2(:,1),order,frame);
fd2(:,2)=sgolayfilt(pd2(:,2),order,frame);
fd2(:,3)=sgolayfilt(pd2(:,3),order,frame);
%fd(:,1)=fd(:,1)-mean(fd(:,1));
%fd(:,2)=fd(:,2)-mean(fd(:,2));
%fd(:,3)=fd(:,3)-mean(fd(:,3));

plot(t,fd(:,1),t,fd(:,2),t,fd(:,3));

%%
figure
NFFT=2^10;
L=length(fd(:,1));
X=fftshift(fft(fd(:,1),NFFT));
Px=X.*conj(X)/(NFFT*L);
fVals=fs*(-NFFT/2:NFFT/2-1)/NFFT; 
semilogy(fVals,Px,'b');

