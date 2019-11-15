
myid = 9999
direction = 0
distance = 0
isMove = 0

function set_uid(id)
	myid = id;
end

function set_direction(dir)
	direction = dir;
end

function event_player_move(player)
	player_x = API_get_x(player);
	player_y = API_get_y(player);
	my_x = API_get_x(myid);
	my_y = API_get_y(myid);
	if (player_x == my_x) then
		if (player_y == my_y) then
			if (isMove == 0) then
				isMove = 1;	
				API_SendMessage(player, myid, "HELLO");
				API_AddFleeEvent(player);
			end
		end
	end
end

function flee(player)
	if (isMove == 1) then
		my_x = API_get_x(myid);
		my_y = API_get_y(myid);
		if (direction == 0) then
			my_y = my_y + 1;
		elseif (direction == 1) then
			my_y = my_y - 1;
		elseif (direction == 2) then
			my_x = my_x - 1;
		elseif (direction == 3) then
			my_x = my_x + 1;
		end
		API_SetPosition(my_x, my_y);
		distance = distance + 1;
		API_AddFleeEvent(player);
		if (distance >= 3) then
			API_SendMessage(player, myid, "BYE");
			isMove = 0;
			distance = 0;
		end
	end
end