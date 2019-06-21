$(document).ready(function(){
    var jsonPath = "load_json/runtime-infos/monitor-data.json";
    const refreshInterval = 1000; //In ms
    const numOfType = 4;         //Number of type to display
    const maxItem   = 120;        //Maximun number of item in each json file
    const chartColors = ['#70ad47', '#ed7d31', '#5b9bd5', '#c93939'];
    var timelineh = $('#timeline').height();
    var chartheight = timelineh - 100;
    var chart = new G2.Chart({
          container: 'chartView',
          forceFit: true,
          height: chartheight,
          animate: false,
          padding: ["7", '18', '30', '50']
        });
    chart.scale('time', {
            tickInterval: maxItem
          });
    chart.scale('value', {
            max:1,
            min:0,
            formatter: val=>{ return val * 100 + "%";},
        });
    chart.axis('value', {
          label: {
                  textStyle: {
                            fill: '#000000'
                          }
                },
          line:{ stroke: 'grey' }
        });
    chart.axis('time', {
          label: {
                  textStyle: {
                            fill: '#000000'
                          }
                }
        });
    chart.tooltip(false);
    chart.line().position('time*value').color('type', chartColors).size(1);

    var items = {};
    chart.legend({
          useHtml: true,
          position: 'bottom',
          reactive: true,
          spaceX: 2,
          containerTpl: '<div class="g2-legend"><div class="g2-legend-list"></div></div>',
          itemTpl: function itemTpl(value, color, checked, index) {
            var markerDom = '<div class="legend-item-marker" style="background-color:' + color + '">' + (index + 1) + '</div>';
            var nameDom = '<div class="legend-item-name">' + value + '</div>';
            var percentDom = '<div class="legend-item-percent">' + items[value] + '</div>';
            return '<div class="g2-legend-list-item">' + markerDom + nameDom + percentDom + '</div>';
          },
          'g2-legend-list-item': {
             marginRight: '0px'
           }
        });
    chart.render();
    ENV.chart = chart;

    var updateChart = function(){
        $.getJSON(jsonPath, function(data){
            if (data.length == 0)
                return;
            index = 0;
            for(let i = 0; i < data.length / numOfType; i++){
                for(let j = 0; j < numOfType; j++){
                    data[index].time = i;
                    index ++;
                }
            }
            index -= numOfType;
            // Update Current Value for each type
            for(j = 0; j < numOfType; j ++){
                items[data[index].type] = (data[index].value*100).toFixed(2) + "%";
                index++;
            }
            chart.changeData(data);
      });
    };
    setInterval(updateChart, refreshInterval);
});
