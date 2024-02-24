LIBRARY ieee;
USE ieee.std_logic_1164.all;

ENTITY LEDRControll IS
	PORT (gpioinput : in STD_LOGIC_VECTOR (3 downto 0); 
			ledoutput: out STD_LOGIC_VECTOR (9 downto 0));
END LEDRControll;

ARCHITECTURE Behavioral of LEDRControll is
begin
	process(gpioinput)
	BEGIN
		case gpioinput is
			when "0000" => ledoutput <= "0000000000";
			when "0001" => ledoutput <= "0000000001";
			when "0010" => ledoutput <= "0000000011";
			when "0011" => ledoutput <= "0000000111";
			when "0100" => ledoutput <= "0000001111";
			when "0101" => ledoutput <= "0000011111";
			when "0110" => ledoutput <= "0000111111";
			when "0111" => ledoutput <= "0001111111";
			when "1000" => ledoutput <= "0011111111";
			when "1001" => ledoutput <= "0111111111";
			when others => ledoutput <= "1111111111";
		end case;
	end process;
end Behavioral;