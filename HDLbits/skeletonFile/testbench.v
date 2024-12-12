`timescale 1ns/1ps

module testbench;
    reg a;
    reg b;
    wire c;

    // 实例化被测试的模块
    example uut (
        .a(a),
        .b(b),
        .c(c)
    );

    initial begin
        // 创建 VCD 文件，供波形查看器使用
        $dumpfile("testbench.vcd");
        $dumpvars(0, testbench);

        // 测试向量 10ns
        a = 0; b = 0; #10;
        a = 0; b = 1; #10;
        a = 1; b = 0; #10;
        a = 1; b = 1; #10;

        // 结束仿真
        $finish;
    end
endmodule
