import json
import random

NAME = 'query2'

queries_2 = '''
166000,199000
175000,198000
55537,43324
88959,96072
172570,197762
52408,165221
128486,140634
189626,210468
190059,201621
112000,130000
'''

datas = []
LB = 1000
UB = 2000

for j, query in enumerate(queries_2.split(sep='\n')):
    for i in range(1):
        try:
            start, target = query.split(sep=',')
        except:
            continue
        
        if random.choice([True, False]):
            new_start = int(start) + int(random.uniform(LB, UB))
        else:
            new_start = int(start) - int(random.uniform(LB, UB))
            
        if random.choice([True, False]):
            new_target = int(target) + int(random.uniform(LB, UB))
        else:
            new_target = int(target) - int(random.uniform(LB, UB))
            
        if random.choice([True, False]):
            datas.append({
                'name': f'{NAME}_{i}_{j}',
                'start_data': new_start,
                'end_data': new_target,
            })
        else:
            datas.append({
                'name': f'{NAME}_{i}_{j}',
                'start_data': new_target,
                'end_data': new_start,
            })
    
with open(NAME + '.json', 'w') as f:
    json.dump(datas, f, indent=4)