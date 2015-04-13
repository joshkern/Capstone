clear all
clc


dataport = serial('COM19', 'Baudrate', 115200, 'Timeout' , 15000, 'InputBufferSize', 51200);
fopen(dataport);
fscanf(dataport);
msg = fread(dataport);
fclose(dataport);
%%
data = dec2bin(msg);

data = reshape(data',1,length(msg)*8);
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

processeddata = data(i-16:length(data));
processeddata = processeddata(1:length(processeddata)-rem(length(processeddata),80));
pd = reshape(processeddata,16,length(processeddata)/16)';
%%

Vc=3.3;
acc=12;
division = 3.3/(2^12);
pd_dec = bin2dec(pd);
pd_dec_v = pd_dec*division;
x=reshape(pd_dec_v,5,(length(pd_dec_v))/5)';
fs=500;
ts = 1/fs;
t = ts:ts:ts*length(x);
hold on
plot(t,x(:,2),t,x(:,3),t,x(:,4),t,x(:,5));
%%
figure
B = 1/50*ones(50,1);
out = filter(B,1,x(:,3));

out1 = filter(B,1,x(:,2));

out2 = filter(B,1,x(:,4));

plot(t,out,out1,out2 )
%%
figure
NFFT=2^10;
L=length(out);
X=fftshift(fft(out,NFFT));
Px=X.*conj(X)/(NFFT*L);
fVals=fs*(-NFFT/2:NFFT/2-1)/NFFT; 
semilogy(fVals,Px,'b');  