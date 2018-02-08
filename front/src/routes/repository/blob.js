import React from 'react';
import ProjectTemplate from './_project';
import { connect } from 'react-redux';
import HighLight from 'react-syntax-highlight';
import ReactMarkdown from 'react-markdown';
import { Card } from 'antd';

class Page extends ProjectTemplate {
    state = { prefixURI: '', path: '', type: 'default', linesCount: 0 }
    
    freshBlob(path, branchName) {
        this.props.dispatch({
            type: 'blob/getTextBlob',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName,
                path
            }
        });
    }

    freshTree(path, branchName) {
        this.props.history.push(this.state.prefixURI  + '/tree' + path);
        this.props.dispatch({
            type: 'tree/getTree',
            payload: {
                repositoriesName: this.props.match.params.repositoriesName,
                repositoryName: this.props.match.params.repositoryName,
                branchName,
                path
            }
        });
    }

    calculateLinesCount(content) {
        let lines = 0;
        for (let i = 0; i < content.length; i++) {
            if (content[i] === '\n')
            {
                lines += 1;
            }
        }
        return lines;
    }

    componentDidMount() {
        let pathItems = window.location.hash.substring(1).split('/blob');
        this.setState({ prefixURI: pathItems[0] });

        let path = '/';
        if (pathItems.length === 2) {
            path = pathItems[1];
        }
        this.setState({
            path,
            prefixURI: pathItems[0],
            linesCount: this.calculateLinesCount(this.props.content),
            type: this.getBlobType(path)
        }, () => this.freshBlob(path, this.props.match.params.branchName));
    }

    componentWillReceiveProps(props) {
        this.setState({ linesCount: this.calculateLinesCount(props.content) });
    }
    
    getBlobType(path) {
        let middleCutResult = path.split('.');
        if (middleCutResult.length > 1) {
            switch (middleCutResult[middleCutResult.length - 1]) {
                case 'cs': return 'csharp';
                case 'xml': return 'xml';
                case 'html': return 'html';
                case 'json': return 'json';
                case 'md': return 'markdown';
                default: return middleCutResult[middleCutResult.length - 1];
            }
        }
        return 'default';
    }

    renderPath() {
        let pathItems = this.state.path.split('/');
        pathItems[0] = { name: '', path: '' };
        for (let i = 1; i < pathItems.length; i++) {
            pathItems[i] = { name: pathItems[i], path: pathItems[i - 1].path + '/' + pathItems[i] };
        }

        pathItems[0].name = this.props.match.params.repositoryName;
        pathItems[0].path = '/';

        return pathItems.map((item, index) => <span style={{ fontSize: 21 }} key={ index }>
            { index === 0 ? '' : '/' }
            <span style={{ margin: '0 4px' }}>
                {
                    index === pathItems.length - 1 && index !== 0
                    ? item.name
                    : <a
                        onClick={() => this.freshTree(item.path, this.props.match.params.branchName)}>
                        { item.name }
                    </a>
                }
            </span>
        </span>);
    }


    unit() {
        return <Card        
            title={ <span>{ this.renderPath() }</span> }
            bodyStyle={{ padding: 20 }}>
            {(() => {
                switch (this.state.type) {
                    case 'markdown': return <ReactMarkdown source={ this.props.content } />
                    default: return <div>
                        <div style={{ float: 'left' }}>
                            <ul style={{
                                listStyle: 'none',
                                textAlign: 'right',
                                padding: '0 5px',
                                marginRight: '12px'
                            }}>
                            {
                                (() => {
                                    let result = [];
                                    for(let index = 0; index <= this.state.linesCount; index++) { result.push(index + 1); }
                                    return result;
                                })().map(item => <li key={ item }><a>{ item }</a></li>)
                            }
                            </ul>
                        </div>
                        <HighLight lang={ this.state.type } value={ this.props.content } />
                    </div>
                }
            })()}
            
        </Card>
    }
}

export default connect(
    ({ blob, branches }) => ({
        entryUnit: 'repositories',
        projectEntryUnit: 'code',
        content: blob.content,
        branches: branches.branches
    })
)(Page);