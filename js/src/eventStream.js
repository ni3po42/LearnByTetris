export class EventStream {

    _queue = [];
    _deferedResolve = null;

    _signal(halt) {
        if (this._deferedResolve) {                        
            this._deferedResolve(!halt);
            this._deferedResolve = null;
        }
    }

    _event(){
        return new Promise(r => this._deferedResolve = r);
    }

    emit(value) {
        this._signal();
        this._queue.push(value); 
    }

    stop() {
        this._queue = [];
        this._signal(true);
    }   

    async *start() {
        this.stop();
        while(await this._event()) {

            while(this._queue.length > 0){
                yield this._queue.shift();
            }

        }
    }

}