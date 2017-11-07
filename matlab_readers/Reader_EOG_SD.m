%clear all;  %clc;
close all; 
%% mudar somente estas variáveis
paciente = 5%5;
hora = 3%3;
plot_all = 0; %plota todas as variaveis em diferentes graficos se = a 1

%% Inicialização das variáveis
bytes = 0;
raw_eog = 0; raw_audio = 0; raw_acc_x = 0; raw_acc_y = 0;  raw_acc_z = 0;
raw_lum_opt = 0;  raw_lum_ldr = 0; raw_bat = 0; raw_umd_hdc = 0; 
raw_temp_hdc = 0; raw_lum_lm_1 = 0; raw_lum_lm_2 = 0;

rec_audio = 0;rec_acc_x = 0;rec_acc_y = 0;rec_acc_z = 0;rec_opt = 0;
rec_ldr = 0;rec_bat = 0;rec_umd_hdc = 0;rec_temp_hdc = 0;rec_temp_lm1 = 0;
rec_temp_lm2 = 0;rec_eog = 0;
%valores adquiridos em teste
zero_g_x = 2240; um_g_x = 2625; coef_a_g_x = 9.82/(um_g_x-zero_g_x);
zero_g_y = 2205; um_g_y = 2609; coef_a_g_y = 9.82/(um_g_y-zero_g_y);
zero_g_z = 2314; um_g_z = 2684; coef_a_g_z = 9.82/(um_g_z-zero_g_z);

one_second_MD_EOG = 1694;

%% Abrindo o arquivo
disp('Abrindo o arquivo')
file_name = sprintf('./files_EOG/EOGP%dH%02d.RAW',paciente,hora);
fileID = fopen(file_name);
if(fileID == -1)
    disp('Arquivo não encontrado')
    return;
else
    rec_message = fread(fileID,'uint8');
end
fclose('all');
size_message = length(rec_message);
segundos = round(size_message/one_second_MD_EOG);
msg = sprintf('Esta placa armazenou %d segundos',segundos);
disp(msg)

%% Decodificando o arquivo
cont_exit = 1;
headers(1) = 0; headers(2) = 0;
while(cont_exit<=length(rec_message))
    f_headers_ok = 0;
    cont_problem = 0; jump_problem = 0;
    while (f_headers_ok == 0) %procurando pelos headers
        headers(1) = headers(2);
        headers(2) = rec_message(cont_exit);
        cont_exit = cont_exit + 1;
        cont_problem = cont_problem+1;
        if((headers(1)==83)&&(headers(2)==84))  
                f_headers_ok = 1;
        end
        if (cont_problem > (3+jump_problem))
           disp(sprintf('Segundo %d NÃO está OK, problema com os headers',round(cont_exit/one_second_MD_EOG)));
           cont_problem = 0; jump_problem = 200;
        end
    end %end of checking headers situation
    
    %lendo um segundo de gravação
    buffer_rec = rec_message(cont_exit:cont_exit + one_second_MD_EOG-3);
    cont_exit = cont_exit + one_second_MD_EOG-2;
    %checando a situação dos CRC, +83+84 representa os headers "ST"
    sum_check = double(sum(double(buffer_rec(1:one_second_MD_EOG-4)))) + 83 +84;
    div = floor(sum_check/65536); %o matlab satura em 65535
    crc_check = sum_check - div*65536;%precisamos então dividir e subtrair
    CRC_1 = double(buffer_rec(one_second_MD_EOG-3)); CRC_2 = double(buffer_rec(one_second_MD_EOG-2));
    CRC = CRC_1*2^8+CRC_2+1; %(+1) pois o programa inicia a soma com 0xffff
    
    if(crc_check == CRC)
        %separing the data into different buffers if CRC is correct
        %disp(sprintf('Segundo %d está OK',round(cont_exit/one_second_MD_EOG)));
        raw_eog = uint32(buffer_rec(1:1200)); 
        raw_audio = buffer_rec(1201:1400); 
        raw_acc_x = buffer_rec(1401:1480); 
        raw_acc_y = buffer_rec(1481:1560);  
        raw_acc_z = buffer_rec(1561:1640);
        raw_lum_opt = buffer_rec(1641:1660); 
        raw_lum_ldr = buffer_rec(1661:1680); 
        raw_bat = buffer_rec(1681:1682);
        raw_umd_hdc = buffer_rec(1683:1684); 
        raw_temp_hdc = buffer_rec(1685:1686); 
        raw_temp_lm1 = buffer_rec(1687:1688); 
        raw_temp_lm2 = buffer_rec(1689:1690);
    else
        %making all equals zero if CRC is wrong: not a good idea
        disp(sprintf('Segundo %d NÃO está OK',round(cont_exit/one_second_MD_EOG)));
%         raw_eog = uint32(ones(1,1200)).*0; 
%         raw_audio = buffer_rec(1201:1400).*0; 
%         raw_acc_x = buffer_rec(1401:1480).*0; 
%         raw_acc_y = buffer_rec(1481:1560).*0;  
%         raw_acc_z = buffer_rec(1561:1640).*0;
%         raw_lum_opt = buffer_rec(1641:1660).*0; 
%         raw_lum_ldr = buffer_rec(1661:1680).*0; 
%         raw_bat = buffer_rec(1681:1682).*0;
%         raw_umd_hdc = buffer_rec(1683:1684).*0; 
%         raw_temp_hdc = buffer_rec(1685:1686).*0; 
%         raw_temp_lm1 = buffer_rec(1687:1688).*0; 
%         raw_temp_lm2 = buffer_rec(1689:1690).*0;
    end
    %result = (uint16_t)(0x0fff & (uint16_t)(result1<<8|result2));
    %exponent = (result1 & 0xf0) >> 4;
    %lux_float = ((float)(result * 0.01) * pow(2, exponent));	//convertion for lux units
    aux_opt = swapbytes(typecast(uint8(raw_lum_opt), 'uint16'));
    mantica = double(bitand(aux_opt,4095));
    expoente = double(bitshift(aux_opt,-12));
    opt_lux = ((mantica.*0.01).*2.^expoente);

    %converting the humidity and the temperature into % and Celsius
    %ranges
    hdc_temp = double(swapbytes(typecast(uint8(raw_temp_hdc), 'uint16')));
    hdc_temp = (hdc_temp/2^16)*165-40;    
    
    hdc_umd = double(swapbytes(typecast(uint8(raw_umd_hdc), 'uint16')));
    hdc_umd = (hdc_umd/2^16)*100;

    %converting the inputs in the original formats (uint16 or 32)
    for i=1:400
        aux_eog(i,1) = (raw_eog(i*3-2)*2^16+ raw_eog(i*3-1)*2^8+ raw_eog(i*3));
    end
    rec_eog = [rec_eog;aux_eog];
    rec_audio = [rec_audio;swapbytes(typecast(uint8(raw_audio), 'uint16'))];
    rec_acc_x = [rec_acc_x;swapbytes(typecast(uint8(raw_acc_x), 'uint16'))];
    rec_acc_y = [rec_acc_y;swapbytes(typecast(uint8(raw_acc_y), 'uint16'))];
    rec_acc_z = [rec_acc_z;swapbytes(typecast(uint8(raw_acc_z), 'uint16'))];
    rec_opt = [rec_opt;opt_lux];
    rec_ldr = [rec_ldr;swapbytes(typecast(uint8(raw_lum_ldr), 'uint16'))];
    rec_bat = [rec_bat;swapbytes(typecast(uint8(raw_bat), 'uint16'))];
    rec_umd_hdc = [rec_umd_hdc;hdc_umd];
    rec_temp_hdc = [rec_temp_hdc;hdc_temp];
    rec_temp_lm1 = [rec_temp_lm1;swapbytes(typecast(uint8(raw_temp_lm1), 'uint16'))];
    rec_temp_lm2 = [rec_temp_lm2;swapbytes(typecast(uint8(raw_temp_lm2), 'uint16'))];
end
%% Ajuste dos vetores e filtros
%excluindo o zero da primeira posição
rec_audio(1) = [];rec_acc_x(1) = [];rec_acc_y(1) = [];rec_acc_z(1) = [];
rec_opt(1) = [];rec_ldr(1) = [];rec_bat(1) = [];rec_umd_hdc(1) = [];
rec_temp_hdc(1) = [];rec_temp_lm1(1) = [];rec_temp_lm2(1) = [];rec_eog(1) = [];

rec_acc_x = double(rec_acc_x);
rec_acc_y = double(rec_acc_y);
rec_acc_z = double(rec_acc_z);

g_acc_x = (rec_acc_x-zero_g_x).*coef_a_g_x;
g_acc_y = (rec_acc_y-zero_g_y).*coef_a_g_y;
g_acc_z = (rec_acc_z-zero_g_z).*coef_a_g_z;

%filtros
rec_eog = double(rec_eog);
[b,a] = butter(2,0.00015,'high');
Dump = ones(50000,1); Dump = Dump.*rec_eog(1);
eog_filt = [Dump;rec_eog]; eog_filt = filter(b,a,eog_filt);
eog_filt = eog_filt(50001:end);


[b,a] = butter(2,0.25,'low');
Dump = ones(50000,1); Dump = Dump.*g_acc_x(1);
filt_acc_x_eog = [Dump;g_acc_x]; filt_acc_x_eog = filter(b,a,filt_acc_x_eog);
filt_acc_x_eog = filt_acc_x_eog(50001:end);

Dump = ones(50000,1); Dump = Dump.*g_acc_y(1);
filt_acc_y_eog = [Dump;g_acc_y]; filt_acc_y_eog = filter(b,a,filt_acc_y_eog);
filt_acc_y_eog = filt_acc_y_eog(50001:end);

Dump = ones(50000,1); Dump = Dump.*g_acc_z(1);
filt_acc_z_eog = [Dump;g_acc_z]; filt_acc_z_eog = filter(b,a,filt_acc_z_eog);
filt_acc_z_eog = filt_acc_z_eog(50001:end);

%% Applying calibration fit on the LM35, audio and LDR data
p_LM = [0.07419   -17.12]; %fator de conversão da temperatura do LM35 e do HDC
LM35_temp = p_LM(1)*double(rec_temp_lm2) + p_LM(2);

p_LDR = [0.04003 -91.07];
LRD_lux = p_LDR(1)*double(rec_ldr) + p_LDR(2);

%% Plots

eog_time = 0:1/400:segundos-1/400; eog_time = eog_time';
sound_time = 0:1/100:segundos-1/100; sound_time = sound_time';
acc_time = 0:1/40:segundos-1/40; acc_time = acc_time';
lux_time = 0:1/10:segundos-1/10; lux_time = lux_time';
temp_time = 0:1:segundos-1; temp_time = temp_time';

if(plot_all)
    figure; 
    subplot(211); plot(eog_time, eog_filt); axis([0 segundos -inf inf]); ylabel('EOG filtrado'); grid on;
    subplot(212); plot(eog_time, rec_eog); axis([0 segundos -inf inf]); ylabel('EOG NÂO filtrado');
    grid on; xlabel('Tempo [s]'); 
    figure; 
    plot(sound_time,rec_audio);title('Audio');xlabel('Tempo [s]');
    figure; 
    subplot(2,1,1); plot(lux_time,rec_ldr);title('Iluminância LDR');
    subplot(2,1,2); plot(lux_time,rec_opt);title('Iluminância OPT3001 [lux]');xlabel('Tempo [s]');
    figure; 
    plot(temp_time,double(rec_bat)/100);ylabel('Bateria [V]');xlabel('Tempo [s]');
    figure; 
    subplot(3,1,1);plot(acc_time,filt_acc_x_eog);ylabel('Eixo X [m/s²]'); grid on;
    subplot(3,1,2);plot(acc_time,filt_acc_y_eog);ylabel('Eixo Y [m/s²]'); grid on;
    subplot(3,1,3);plot(acc_time,filt_acc_z_eog);ylabel('Eixo Z [m/s²]'); grid on; xlabel('Tempo [s]');
    figure; 
    subplot(3,1,1);plot(temp_time,rec_temp_hdc);ylabel('Temp. HDC [°C]');
    subplot(3,1,2);plot(temp_time,rec_temp_lm1);title('Temperatura LM35 - 1');
    subplot(3,1,3);plot(temp_time,rec_temp_lm2);title('Temperatura LM35 - 2'); xlabel('Tempo [s]');
    figure; 
    plot(temp_time,rec_umd_hdc);ylabel('Umidade Ambiente[%]');
end


figure;
subplot(4,2,1); plot(eog_time,eog_filt);ylabel('EOG'); grid on; axis([0 segundos -inf inf])
subplot(4,2,3); plot(sound_time,rec_audio);ylabel('Audio'); grid on; axis([0 segundos -inf inf])
subplot(4,2,5); plot(lux_time,rec_opt);ylabel('Ilum. [lux]');grid on; axis([0 segundos -inf inf])
subplot(4,2,7); plot(temp_time,rec_temp_hdc);ylabel('Temp. [°C]');grid on; axis([0 segundos -inf inf]); xlabel('Tempo [s]'); hold on;
                plot(temp_time,LM35_temp);
subplot(4,2,2); plot(temp_time,rec_umd_hdc);ylabel('Umidade[%]');grid on; axis([0 segundos -inf inf])
subplot(4,2,4); plot(acc_time,filt_acc_x_eog);ylabel('Acc. X [m/s²]'); grid on; axis([0 segundos -inf inf])
subplot(4,2,6); plot(acc_time,filt_acc_y_eog);ylabel('Acc. Y [m/s²]'); grid on; axis([0 segundos -inf inf])
subplot(4,2,8); plot(temp_time,double(rec_bat)/100);ylabel('Bateria [V]');grid on; xlabel('Tempo [s]'); axis([0 segundos -inf inf])
suptitle(sprintf('MD-EOG Paciente%d, Hora %02d',paciente,hora));

